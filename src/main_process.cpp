/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "main_process.hpp"

#include <btu/common/filesystem.hpp>
#include <btu/hkx/anim.hpp>
#include <btu/nif/mesh.hpp>
#include <btu/nif/optimize.hpp>
#include <btu/tex/optimize.hpp>
#include <btu/tex/texture.hpp>

namespace cao {
auto guess_file_type(const std::filesystem::path &path) noexcept -> FileType
{
    const auto extension = path.extension().u8string();
    if (extension == u8".nif")
        return FileType::Mesh;
    if (extension == u8".dds" || extension == u8".tga")
        return FileType::Texture;
    if (extension == u8".hkx")
        return FileType::Animation;
}

[[nodiscard]] auto process_mesh(btu::modmanager::ModFolder::ModFile &&file,
                                const btu::nif::Settings &settings) noexcept
    -> tl::expected<std::vector<std::byte>, btu::common::Error>
{
    return btu::nif::load(std::move(file.relative_path), file.content)
        .and_then([&](auto &&nif) {
            auto steps = btu::nif::compute_optimization_steps(nif, settings);
            return btu::nif::optimize(BTU_FWD(nif), steps);
        })
        .and_then([](auto &&nif) { return btu::nif::save(BTU_FWD(nif)); });
}

[[nodiscard]] auto process_texture(btu::modmanager::ModFolder::ModFile &&file,
                                   const btu::tex::Settings &settings) noexcept
    -> tl::expected<std::vector<std::byte>, btu::common::Error>
{
    static auto thread_local compression_device = btu::tex::CompressionDevice::make(
        0); // TODO: make this configurable

    return btu::tex::load(std::move(file.relative_path), file.content)
        .and_then([&](auto &&tex) {
            auto steps = btu::tex::compute_optimization_steps(tex, settings);
            return btu::tex::optimize(BTU_FWD(tex), steps, compression_device);
        })
        .and_then([](auto &&tex) { return btu::tex::save(BTU_FWD(tex)); });
}

[[nodiscard]] auto process_animation(btu::modmanager::ModFolder::ModFile &&file,
                                     std::optional<btu::Game> hkx_target) noexcept
    -> tl::expected<std::vector<std::byte>, btu::common::Error>
{
#ifdef _WIN32
    if (!hkx_target)
        return {};                         // TODO

    return btu::hkx::AnimExe::make("data") // TODO: make this configurable
        .and_then([&](btu::hkx::AnimExe &&exe) { return exe.convert(*hkx_target, file.content); });
#else
    return tl::make_unexpected(btu::common::Error(std::make_error_code(std::errc::not_supported)));
#endif
}

auto process_file(btu::modmanager::ModFolder::ModFile &&file, const Settings &settings) noexcept
    -> tl::expected<std::vector<std::byte>, btu::common::Error>
{
    const auto type       = guess_file_type(file.relative_path);
    const auto &file_sets = settings.current_profile().get_per_file_settings(file.relative_path);
    switch (type)
    {
        case FileType::Mesh: return process_mesh(std::move(file), file_sets.nif);
        case FileType::Texture: return process_texture(std::move(file), file_sets.tex);
        case FileType::Animation: return process_animation(std::move(file), file_sets.hkx_target);
    }
    assert(false && "process_file: unreachable");
}
} // namespace cao
