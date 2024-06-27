/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "settings/settings.hpp"

#include <btu/modmanager/mod_folder.hpp>

namespace cao {
enum class FileType : std::uint8_t
{
    Mesh,
    Texture,
    Animation,
};

[[nodiscard]] auto guess_file_type(const std::filesystem::path &path) noexcept -> std::optional<FileType>;

const static auto k_error_no_work_required = std::error_code(0, std::generic_category());
const static auto k_unreachable            = std::error_code(1, std::generic_category());

[[nodiscard]] auto process_file(btu::modmanager::ModFolder::ModFile &&file, const Settings &settings) noexcept
    -> tl::expected<std::vector<std::byte>, btu::common::Error>;
} // namespace cao
