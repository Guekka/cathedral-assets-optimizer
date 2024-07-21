/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "main_process.hpp"

#include <btu/common/games.hpp>
#include <btu/common/metaprogramming.hpp>
#include <btu/common/string.hpp>
#include <btu/hkx/anim.hpp>
#include <btu/nif/detail/common.hpp>
#include <btu/nif/mesh.hpp>
#include <btu/nif/optimize.hpp>
#include <btu/tex/optimize.hpp>
#include <btu/tex/texture.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <plog/Log.h>
#include <tl/expected.hpp>

#include <string_view>

template<>
struct fmt::formatter<btu::Game> : fmt::formatter<std::string_view>
{
    [[maybe_unused]] auto format(const btu::Game &game, format_context &ctx)
    {
        const auto *game_str = [game] {
            switch (game)
            {
                case btu::Game::TES3: return "Morrowind";
                case btu::Game::TES4: return "Oblivion";
                case btu::Game::FNV: return "Fallout New Vegas";
                case btu::Game::SLE: return "Skyrim LE";
                case btu::Game::SSE: return "Skyrim SE";
                case btu::Game::FO4: return "Fallout 4";
                case btu::Game::Custom: return "Custom";
                default:
                {
                    assert(false && "formatter<btu::Game>: unreachable");
                    return "Invalid";
                }
            }
        }();

        return fmt::formatter<std::string_view>::format(game_str, ctx);
    }
};

template<>
struct fmt::formatter<DXGI_FORMAT> : fmt::formatter<std::string_view>
{
    [[maybe_unused]] auto format(const DXGI_FORMAT &format, format_context &ctx)
    {
        return fmt::formatter<std::string_view>::format(btu::common::as_ascii(btu::tex::to_string(format)),
                                                        ctx);
    }
};

namespace cao {
auto guess_file_type(const std::filesystem::path &path) noexcept -> std::optional<FileType>
{
    const auto extension = path.extension().u8string();
    if (extension == u8".nif")
        return FileType::Mesh;
    if (extension == u8".dds" || extension == u8".tga")
        return FileType::Texture;
    if (extension == u8".hkx")
        return FileType::Animation;

    return std::nullopt;
}

[[nodiscard]] auto steps_are_empty(const btu::nif::OptimizationSteps &steps) noexcept -> bool
{
    return !steps.format && !steps.rename_referenced_textures
           && steps.headpart == btu::nif::HeadpartStatus::No && !steps.optimize;
}

[[nodiscard]] auto steps_are_empty(const btu::tex::OptimizationSteps &steps) noexcept -> bool
{
    return !steps.convert && !steps.mipmaps && !steps.add_transparent_alpha && !steps.resize;
}

[[nodiscard]] auto human_readable_step_string(const btu::nif::OptimizationSteps &steps) noexcept
    -> std::string
{
    auto strs = std::vector<std::string>{};

    if (steps.format)
        strs.emplace_back(fmt::format("format: {}", steps.format.value()));
    if (steps.rename_referenced_textures)
        strs.emplace_back("rename textures tga -> dds");
    if (steps.headpart == btu::nif::HeadpartStatus::Yes)
        strs.emplace_back("processing as headpart");
    if (steps.optimize)
        strs.emplace_back("optimizing");

    return fmt::format("{}", fmt::join(strs, ", "));
}

[[nodiscard]] auto human_readable_step_string(const btu::tex::OptimizationSteps &steps) noexcept
    -> std::string
{
    auto strs = std::vector<std::string>{};

    if (steps.convert)
        strs.emplace_back(fmt::format("convert to {}", steps.best_format));
    if (steps.mipmaps)
        strs.emplace_back("generate mipmaps");
    if (steps.add_transparent_alpha)
        strs.emplace_back("add transparent alpha");
    if (steps.resize)
        strs.emplace_back(fmt::format("resize to {}x{}", steps.resize->w, steps.resize->h));

    return fmt::format("{}", fmt::join(strs, ", "));
}

void log_file_processing(const std::filesystem::path &path, std::string_view steps) noexcept
{
    PLOGI << fmt::format("Processing file: {}. Steps: {}", path.string(), steps);
}

void log_file_no_work_required(const std::filesystem::path &path) noexcept
{
    PLOGV << fmt::format("No work required for file: {}", path.string());
}

[[nodiscard]] auto process_mesh(btu::modmanager::ModFile &&file,
                                const btu::nif::Settings &settings,
                                const OptimizeType type) noexcept
    -> tl::expected<std::vector<std::byte>, btu::common::Error>
{
    if (type == OptimizeType::None)
        return tl::make_unexpected(btu::common::Error(k_error_no_work_required));

    return file.content
        ->and_then(
            [&file](std::vector<std::byte> content) { return btu::nif::load(file.relative_path, content); })
        .and_then([&](auto &&nif) -> tl::expected<btu::nif::Mesh, btu::common::Error> {
            auto steps = btu::nif::compute_optimization_steps(nif, settings);

            if (steps_are_empty(steps))
            {
                log_file_no_work_required(file.relative_path);
                return tl::make_unexpected(btu::common::Error(k_error_no_work_required));
            }

            if (type == OptimizeType::Forced)
                steps.format = std::optional(settings.target_game); // force conversion

            log_file_processing(file.relative_path, human_readable_step_string(steps));

            if (type == OptimizeType::DryRun)
                return nif;

            return btu::nif::optimize(BTU_FWD(nif), steps);
        })
        .and_then([type](auto &&nif) -> tl::expected<std::vector<std::byte>, btu::common::Error> {
            if (type == OptimizeType::DryRun)
                return tl::make_unexpected(btu::common::Error(k_error_no_work_required));

            return btu::nif::save(BTU_FWD(nif));
        });
}

[[nodiscard]] auto process_texture(btu::modmanager::ModFile &&file,
                                   const btu::tex::Settings &settings,
                                   const OptimizeType type) noexcept
    -> tl::expected<std::vector<std::byte>, btu::common::Error>
{
    static auto compression_device = btu::tex::CompressionDevice();

    if (type == OptimizeType::None)
        return tl::make_unexpected(btu::common::Error(k_error_no_work_required));

    return file.content
        ->and_then([&file](auto &&content) { return btu::tex::load(file.relative_path, content); })
        .and_then([&](auto &&tex) -> tl::expected<btu::tex::Texture, btu::common::Error> {
            auto steps = btu::tex::compute_optimization_steps(tex, settings);

            if (steps_are_empty(steps))
            {
                log_file_no_work_required(file.relative_path);
                return tl::make_unexpected(btu::common::Error(k_error_no_work_required));
            }

            if (type == OptimizeType::Forced)
                steps.convert = true;

            log_file_processing(file.relative_path, human_readable_step_string(steps));

            if (type == OptimizeType::DryRun)
                return tl::make_unexpected(btu::common::Error(k_error_no_work_required));

            return btu::tex::optimize(BTU_FWD(tex), steps, compression_device);
        })
        .and_then([type](auto &&tex) -> tl::expected<std::vector<std::byte>, btu::common::Error> {
            if (type == OptimizeType::DryRun)
                return tl::make_unexpected(btu::common::Error(k_error_no_work_required));

            return btu::tex::save(BTU_FWD(tex));
        });
}

[[nodiscard]] auto process_animation(btu::modmanager::ModFile &&file,
                                     btu::Game hkx_target,
                                     OptimizeType type) noexcept
    -> tl::expected<std::vector<std::byte>, btu::common::Error>
{
    if (type == OptimizeType::None)
        return tl::make_unexpected(btu::common::Error(k_error_no_work_required));

    auto exe = btu::hkx::AnimExe::make("data"); // TODO: make this configurable
    if (!exe)
        return tl::make_unexpected(exe.error());

    // TODO: better dry run?
    if (type == OptimizeType::DryRun)
    {
        PLOGI << std::format("{} might be optimized", file.relative_path.string());
        return tl::make_unexpected(btu::common::Error(k_error_no_work_required));
    }

    return file.content->and_then(
        [&](std::vector<std::byte> content) { return exe->convert(hkx_target, content); });
}

auto process_file(btu::modmanager::ModFile &&file, const Settings &settings) noexcept
    -> tl::expected<std::vector<std::byte>, btu::common::Error>
{
    const auto type       = guess_file_type(file.relative_path);
    const auto &file_sets = settings.current_profile().get_per_file_settings(file.relative_path);

    if (!type)
        return tl::make_unexpected(btu::common::Error(k_error_no_work_required)); // TODO: better error

    const auto get_optimize_type = [settings](OptimizeType opt_type) {
        auto dry_run         = settings.current_profile().dry_run;
        auto should_optimize = opt_type != OptimizeType::None;
        return (dry_run && should_optimize) ? OptimizeType::DryRun : opt_type;
    };

    switch (type.value())
    {
        case FileType::Mesh:
            return process_mesh(std::move(file), file_sets.nif, get_optimize_type(file_sets.nif_optimize));
        case FileType::Texture:
            return process_texture(std::move(file), file_sets.tex, get_optimize_type(file_sets.tex_optimize));
        case FileType::Animation:
            return process_animation(std::move(file),
                                     file_sets.hkx_target,
                                     get_optimize_type(file_sets.hkx_optimize));
    }
    return tl::make_unexpected(btu::common::Error(k_unreachable));
}
} // namespace cao
