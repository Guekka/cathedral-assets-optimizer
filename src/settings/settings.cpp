/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "settings/settings.hpp"

#include "json.hpp"

#include <flux.hpp>
#include <sago/platform_folders.h>

namespace cao {

void Settings::create_profile(std::u8string profile_name, Profile base_profile) noexcept
{
    if (get_profile(profile_name))
        return;

    profiles_.emplace_back(std::move(profile_name), std::move(base_profile));
}

auto Settings::current_profile() const noexcept -> const Profile &
{
    return profiles_.at(current_profile_index_).second;
}

auto Settings::current_profile() noexcept -> Profile &
{
    return profiles_.at(current_profile_index_).second;
}

auto Settings::current_profile_name() const noexcept -> std::u8string_view
{
    return profiles_.at(current_profile_index_).first;
}

void Settings::remove(std::u8string_view profile) noexcept
{
    auto it = std::ranges::find_if(profiles_, [profile](const auto &p) { return p.first == profile; });
    if (it == profiles_.end())
        return;

    auto current_profile = current_profile_name();
    if (current_profile == profile)
        current_profile_index_ = 0;

    profiles_.erase(it);

    if (current_profile_index_ >= profiles_.size())
        current_profile_index_ = profiles_.size() - 1;
}

auto Settings::set_current_profile(std::u8string_view profile) noexcept -> bool
{
    auto it = std::ranges::find_if(profiles_, [profile](const auto &p) { return p.first == profile; });
    if (it == profiles_.end())
        return false;

    current_profile_index_ = std::distance(profiles_.begin(), it);
    return true;
}

auto Settings::get_profile(std::u8string_view profile_name) const noexcept -> std::optional<Profile>
{
    auto it = std::ranges::find_if(profiles_,
                                   [profile_name](const auto &p) { return p.first == profile_name; });
    if (it == profiles_.end())
        return std::nullopt;

    return it->second;
}

auto Settings::make_base() noexcept -> Settings
{
    Settings settings;
    settings.profiles_ = {
        {u8"SSE", Profile::make_base(btu::Game::SSE)},
        {u8"SLE", Profile::make_base(btu::Game::SLE)},
    };
    settings.current_profile_index_ = 0;

    return settings;
}

using SagoFunction = std::string (*)();

[[nodiscard]] auto any_directory(SagoFunction func, std::filesystem::path default_path) noexcept
    -> std::filesystem::path
{
    try
    {
        auto base = std::filesystem::path(func());

        auto path = base / Settings::k_app_name;
        std::filesystem::create_directories(path);

        return path;
    }
    catch (const std::exception &)
    {
        return std::filesystem::absolute(default_path);
    }
}

auto Settings::config_directory() noexcept -> std::filesystem::path
{
    return any_directory(&sago::getConfigHome, "config");
}

auto Settings::state_directory() noexcept -> std::filesystem::path
{
    return any_directory(&sago::getStateDir, "state");
}

auto Settings::list_profiles() const noexcept -> std::vector<std::u8string_view>
{
    return flux::ref(profiles_)
        .map([](const auto &pair) -> std::u8string_view { return pair.first; })
        .to<std::vector>();
}

auto load_settings() -> Settings
{
    try
    {
        const auto settings_file = Settings::config_directory() / "settings.json";
        return json::read_from_file<Settings>(settings_file);
    }
    catch (const std::exception &)
    {
        return Settings::make_base();
    }
}

auto save_settings(const Settings &settings) -> bool
{
    const auto settings_file = Settings::config_directory() / "settings.json";
    return json::save_to_file(settings, settings_file);
}

auto current_per_file_settings(Settings &sets) noexcept -> PerFileSettings &
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    return const_cast<PerFileSettings &>(current_per_file_settings(std::as_const(sets)));
}

auto current_per_file_settings(const Settings &sets) noexcept -> const PerFileSettings &
{
    const auto &profile          = sets.current_profile();
    const auto &selected_pattern = sets.gui.selected_pattern;

    if (profile.base_per_file_settings.pattern.text() == selected_pattern)
        return profile.base_per_file_settings;

    auto it = std::ranges::find_if(profile.per_file_settings, [&selected_pattern](const auto &pfs) {
        return pfs.pattern.text() == selected_pattern;
    });

    if (it != profile.per_file_settings.end())
        return *it;

    return profile.base_per_file_settings;
}

} // namespace cao
