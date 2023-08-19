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
#include <system_error>

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
            }
            assert(false && "formatter<btu::Game>: unreachable");
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
           && steps.headpart == btu::nif::HeadpartStatus::No;
}

[[nodiscard]] auto steps_are_empty(const btu::tex::OptimizationSteps &steps) noexcept -> bool
{
    return !steps.convert && !steps.mipmaps && !steps.add_transparent_alpha && !steps.resize;
}

void log_steps(const btu::nif::OptimizationSteps &steps) noexcept
{
    auto steps_log = std::string{"Optimizing mesh. Steps: "};
    if (steps.format)
        fmt::format_to(std::back_inserter(steps_log), "format: {}, ", steps.format.value());
    if (steps.rename_referenced_textures)
        steps_log += "rename textures tga -> dds, ";
    if (steps.headpart == btu::nif::HeadpartStatus::Yes)
        steps_log += "processing as headpart";

    PLOG_INFO << steps_log;
}

void log_steps(const btu::tex::OptimizationSteps &steps) noexcept
{
    auto steps_log = std::string{"Optimizing texture. Steps: "};
    if (steps.convert)
        fmt::format_to(std::back_inserter(steps_log), "converting to {}, ", steps.best_format);
    if (steps.mipmaps)
        steps_log += "generating mipmaps, ";
    if (steps.add_transparent_alpha)
        steps_log += "adding transparent alpha, ";
    if (steps.resize)
        fmt::format_to(std::back_inserter(steps_log), "resizing to {}x{}, ", steps.resize->w, steps.resize->h);

    PLOG_INFO << steps_log;
}

[[nodiscard]] auto process_mesh(btu::modmanager::ModFolder::ModFile &&file,
                                const btu::nif::Settings &settings,
                                const OptimizeType type) noexcept
    -> tl::expected<std::vector<std::byte>, btu::common::Error>
{
    if (type == OptimizeType::None)
        return tl::make_unexpected(btu::common::Error(k_error_no_work_required));

    return btu::nif::load(std::move(file.relative_path), *file.content)
        .and_then([&](auto &&nif) -> tl::expected<btu::nif::Mesh, btu::common::Error> {
            auto steps = btu::nif::compute_optimization_steps(nif, settings);

            if (steps_are_empty(steps))
            {
                PLOG_INFO << "No work required";
                return tl::make_unexpected(btu::common::Error(k_error_no_work_required));
            }

            if (type == OptimizeType::Forced)
                steps.format = std::optional(settings.game); // force conversion

            log_steps(steps);
            return btu::nif::optimize(BTU_FWD(nif), steps);
        })
        .and_then([](auto &&nif) { return btu::nif::save(BTU_FWD(nif)); });
}

[[nodiscard]] auto process_texture(btu::modmanager::ModFolder::ModFile &&file,
                                   const btu::tex::Settings &settings,
                                   const OptimizeType type) noexcept
    -> tl::expected<std::vector<std::byte>, btu::common::Error>
{
    static auto thread_local compression_device = btu::tex::CompressionDevice::make(
        0); // TODO: make this configurable

    if (type == OptimizeType::None)
        return tl::make_unexpected(btu::common::Error(k_error_no_work_required));

    return btu::tex::load(std::move(file.relative_path), *file.content)
        .and_then([&](auto &&tex) -> tl::expected<btu::tex::Texture, btu::common::Error> {
            auto steps = btu::tex::compute_optimization_steps(tex, settings);

            if (steps_are_empty(steps))
            {
                PLOG_INFO << "No work required";
                return tl::make_unexpected(btu::common::Error(k_error_no_work_required));
            }

            if (type == OptimizeType::Forced)
                steps.convert = true;

            log_steps(steps);
            return btu::tex::optimize(BTU_FWD(tex), steps, compression_device);
        })
        .and_then([](auto &&tex) { return btu::tex::save(BTU_FWD(tex)); });
}

[[nodiscard]] auto process_animation([[maybe_unused]] btu::modmanager::ModFolder::ModFile &&file,
                                     [[maybe_unused]] std::optional<btu::Game> hkx_target) noexcept
    -> tl::expected<std::vector<std::byte>, btu::common::Error>
{
    if (!hkx_target)
        return {}; // TODO

    return btu::hkx::AnimExe::make("data") // TODO: make this configurable
        .and_then([&](btu::hkx::AnimExe &&exe) { return exe.convert(*hkx_target, *file.content); });
}

auto process_file(btu::modmanager::ModFolder::ModFile &&file, const Settings &settings) noexcept
    -> tl::expected<std::vector<std::byte>, btu::common::Error>
{
    const auto type       = guess_file_type(file.relative_path);
    const auto &file_sets = settings.current_profile().get_per_file_settings(file.relative_path);

    if (!type)
        return tl::make_unexpected(btu::common::Error(k_error_no_work_required)); // TODO: better error

    switch (type.value())
    {
        case FileType::Mesh: return process_mesh(std::move(file), file_sets.nif, file_sets.nif_optimize);
        case FileType::Texture:
            return process_texture(std::move(file), file_sets.tex, file_sets.tex_optimize);
        case FileType::Animation: return process_animation(std::move(file), file_sets.hkx_target);
    }
    assert(false && "process_file: unreachable");
}
} // namespace cao
