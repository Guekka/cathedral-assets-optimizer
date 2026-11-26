/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "settings/json.hpp"
#include "settings/profile.hpp"

#include <stdexcept>

namespace cao::cli {

[[nodiscard]] inline auto load_profile_from_json(const std::filesystem::path &json_path) -> Profile
{
    auto profile = json::read_from_file<Profile>(json_path);
    if (!profile)
    {
        throw std::runtime_error("Failed to load profile from JSON file: " + json_path.string()
                                 + ". Check that the file exists and has a valid profile format.");
    }

    return std::move(*profile);
}

[[nodiscard]] inline auto profile_for_mod_path(const Profile &source,
                                              const std::filesystem::path &mod_path) -> Profile
{
    if (mod_path.empty())
        throw std::runtime_error("Mod paths cannot be empty");

    auto profile              = source;
    profile.input_path        = mod_path;
    profile.optimization_mode = OptimizationMode::SingleMod;
    return profile;
}

} // namespace cao::cli
