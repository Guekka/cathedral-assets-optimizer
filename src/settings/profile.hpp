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
enum class BsaOperation : std::uint8_t
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

    [[nodiscard]] auto per_file_settings() noexcept -> std::vector<PerFileSettings *>
    {
        std::vector<PerFileSettings *> result;
        for (auto &settings : per_file_settings_)
            result.push_back(&settings);

        result.push_back(&base_per_file_settings_);

        return result;
    }

    [[nodiscard]] auto per_file_settings() const noexcept -> std::vector<const PerFileSettings *>
    {
        std::vector<const PerFileSettings *> result;
        for (const auto &settings : per_file_settings_)
            result.push_back(&settings);

        result.push_back(&base_per_file_settings_);

        return result;
    }

    void add_per_file_settings(PerFileSettings settings) noexcept
    {
        per_file_settings_.emplace_back(std::move(settings));
    }

    auto remove_per_file_settings(std::u8string_view text) noexcept -> bool
    {
        const auto it = std::ranges::find(per_file_settings_, text, [](const auto &pfs) {
            return pfs.pattern.text();
        });

        if (it == per_file_settings_.end())
            return false;

        per_file_settings_.erase(it);
        return true;
    }

    [[nodiscard]] auto get_per_file_settings(const std::filesystem::path &path) const noexcept
        -> PerFileSettings
    {
        const auto it = std::ranges::find_if(per_file_settings_, [&path](const auto &settings) {
            return settings.matches(path);
        });

        if (it == per_file_settings_.end())
            return base_per_file_settings_;

        return *it;
    }

    [[nodiscard]] static auto make_base(btu::Game game) noexcept -> Profile
    {
        Profile profile;
        profile.target_game             = game;
        profile.base_per_file_settings_ = PerFileSettings::make_base(game);

        return profile;
    }

private:
    PerFileSettings base_per_file_settings_;
    std::vector<PerFileSettings> per_file_settings_;

public:
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Profile,
                                   bsa_operation,
                                   bsa_make_dummy_plugins,
                                   dry_run,
                                   optimization_mode,
                                   target_game,
                                   input_path,
                                   mods_blacklist,
                                   base_per_file_settings_,
                                   per_file_settings_)
};

NLOHMANN_JSON_SERIALIZE_ENUM(BsaOperation,
                             {{BsaOperation::None, nullptr},
                              {BsaOperation::Create, "create"},
                              {BsaOperation::Extract, "extract"}})

} // namespace cao
