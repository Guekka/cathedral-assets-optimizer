/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "settings/settings.hpp"

#include <btu/modmanager/mod_folder.hpp>

namespace cao {
enum class FileType
{
    Mesh,
    Texture,
    Animation,
};

[[nodiscard]] auto guess_file_type(const std::filesystem::path &path) noexcept -> FileType;

[[nodiscard]] auto process_file(btu::modmanager::ModFolder::ModFile &&file, const Settings &settings) noexcept
    -> tl::expected<std::vector<std::byte>, btu::common::Error>;
} // namespace cao