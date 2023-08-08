/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "base_types.hpp"
#include "per_file_settings.hpp"

#include <btu/common/games.hpp>
#include <btu/common/path.hpp>
#include <nlohmann/json.hpp>

namespace cao {
enum class BsaOperation
{
    None,
    Create,
    Extract,
};

class Profile
{
public:
    BsaOperation bsa_operation  = BsaOperation::None;
    bool bsa_make_dummy_plugins = false;

    bool dry_run = false; // TODO: implement

    OptimizationMode optimization_mode = OptimizationMode::SingleMod;
    btu::Game target_game              = btu::Game::SSE;

    btu::Path input_path;

    std::vector<std::u8string> mods_blacklist;

    PerFileSettings base_per_file_settings;
    std::vector<PerFileSettings> per_file_settings;

    [[nodiscard]] inline auto get_per_file_settings(const std::filesystem::path &path) const noexcept
        -> PerFileSettings
    {
        auto it = std::ranges::find_if(per_file_settings,
                                       [&path](const auto &settings) { return settings.matches(path); });

        if (it == per_file_settings.end())
            return base_per_file_settings;
        return *it;
    }

    [[nodiscard]] static inline auto make_base(btu::Game game) noexcept -> Profile
    {
        Profile profile;
        profile.target_game            = game;
        profile.base_per_file_settings = PerFileSettings::make_base(game);

        return profile;
    }
};

NLOHMANN_JSON_SERIALIZE_ENUM(BsaOperation,
                             {{BsaOperation::None, nullptr},
                              {BsaOperation::Create, "create"},
                              {BsaOperation::Extract, "extract"}})

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Profile,
                                   bsa_operation,
                                   bsa_make_dummy_plugins,
                                   dry_run,
                                   optimization_mode,
                                   target_game,
                                   input_path,
                                   mods_blacklist,
                                   base_per_file_settings,
                                   per_file_settings)

} // namespace cao
