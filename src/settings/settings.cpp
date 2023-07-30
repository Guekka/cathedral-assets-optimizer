/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "settings/settings.hpp"

#include "json.hpp"

#include <flow.hpp>
#include <sago/platform_folders.h>

namespace cao {

void Settings::create_profile(const std::u8string &profile_name)
{
    // TODO
}

void Settings::create_profile_from_base(const std::u8string &profile_name, const Profile &base_profile)
{
    // TODO
}

auto Settings::current_profile() const noexcept -> const Profile &
{
    return profiles_.at(current_profile_index_).second;
}

auto Settings::current_profile() noexcept -> Profile &
{
    return profiles_.at(current_profile_index_).second;
}

void Settings::remove(size_t index) noexcept
{
    // TODO
}

auto Settings::find_profile(std::u8string_view profile_name) const noexcept -> std::optional<size_t>
{
    return 0; // TODO
}

auto Settings::set_current_profile(size_t index) noexcept -> bool
{
    return false; // TODO
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

auto Settings::data_directory() noexcept -> std::filesystem::path
{
    return any_directory(&sago::getDataHome, "data");
}

auto Settings::config_directory() noexcept -> std::filesystem::path
{
    return any_directory(&sago::getConfigHome, "config");
}

auto Settings::list_profiles() const noexcept -> std::vector<std::u8string_view>
{
    return flow::from(profiles_)
        .map([](const auto &pair) -> std::u8string_view { return pair.first; })
        .to_vector();
}

auto load_settings() -> Settings
{
    const auto settings_file = Settings::config_directory() / "settings.json";
    return json::read_from_file<Settings>(settings_file);
}

auto save_settings(const Settings &settings) -> bool
{
    const auto settings_file = Settings::config_directory() / "settings.json";
    return json::save_to_file(settings, settings_file);
}

void from_json(const nlohmann::json &j, Settings &settings)
{
    // TODO
}

void to_json(nlohmann::json &j, const Settings &settings)
{
    // TODO
}

} // namespace cao
