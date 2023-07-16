/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "settings/settings.hpp"

#include "json.hpp"

#include <sago/platform_folders.h>

namespace cao {

void Settings::create_profile(const std::u8string &profile_name)
{
    // TODO
}

void Settings::create_profile_from_base(const std::u8string &profile_name,
                                        const std::u8string &base_profile_name)
{
    // TODO
}

auto Settings::current_profile() const noexcept -> Profile
{
    // TODO
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
