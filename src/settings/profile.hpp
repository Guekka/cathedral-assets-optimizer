/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "PatternMap.hpp"

namespace cao {
struct Profile
{
    bool bsa_create;
    bool bsa_extract;
    bool bsa_make_dummy_plugins;

    bool is_base_profile;

    // bool dry_run; TODO

    OptimizationMode optimization_mode;
    btu::Game target_game;

    btu::Path input_path;

    std::vector<std::u8string> mods_blacklist;
};
} // namespace cao
