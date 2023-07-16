/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "base_types.hpp"
#include "per_file_settings.hpp"

#include <btu/common/games.hpp>
#include <btu/common/path.hpp>

namespace cao {
class Profile
{
public:
    bool bsa_create;
    bool bsa_extract;
    bool bsa_make_dummy_plugins;

    bool is_base_profile;

    // bool dry_run; TODO

    OptimizationMode optimization_mode;
    btu::Game target_game;

    btu::Path input_path;

    std::vector<std::u8string> mods_blacklist;

    std::vector<PerFileSettings> per_file_settings;

    [[nodiscard]] inline auto get_per_file_settings(const std::filesystem::path &path) const noexcept
        -> PerFileSettings
    {
        for (const auto &perf : per_file_settings)
            if (std::ranges::any_of(perf.patterns,
                                    [&path](const auto &pattern) { return pattern.matches(path); }))
                return perf;

        assert(false && "get_per_file_settings: unreachable"); // TODO: handle this case
    }
};

} // namespace cao
