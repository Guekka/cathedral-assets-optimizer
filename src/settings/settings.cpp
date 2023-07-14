/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "settings/settings.hpp"

#include "JSON.hpp"
#include "Utils/Filesystem.hpp"
#include "manager.hpp"

#include <sago/platform_folders.h>

namespace cao {
/*
Settings::Profiles()
    : Profiles(QDir(defaultProfileDir))
{
}

Settings::Profiles(QDir dir)
    : rootProfileDir_(std::move(dir))
{
    update(true);
    nlohmann::json j;
    JSON::readFromFile(j, commonSettingsPath());
    commonSettings_.setJSON(j);
}

void Settings::create(const std::u8string &name, const std::u8string &baseProfile)
{
    const std::u8string &newFolder = rootProfileDir_.absoluteFilePath(name);
    rootProfileDir_.mkpath(newFolder);
    auto [it, inserted] = profiles_.try_emplace(name, Profile(newFolder));

    if (inserted)
    {
        const Profile &base             = get(exists(baseProfile) ? baseProfile : defaultProfile);
        it->second.getPatterns()        = base.getPatterns();
        it->second.getFileTypes()       = base.getFileTypes();
        it->second.getGeneralSettings() = base.getGeneralSettings();
        it->second.saveToJSON();
    }
}

void Settings::create_profile(const std::u8string &profile_name)
{
    const std::u8string &newFolder = rootProfileDir_.absoluteFilePath(profile_name);
    rootProfileDir_.mkpath(newFolder);
    profiles_.try_emplace(profile_name, Profile(newFolder));
}

void Settings::create(const Profile &profile, const std::u8string &name)
{
    const std::u8string &newFolder = rootProfileDir_.absoluteFilePath(name);
    rootProfileDir_.mkpath(newFolder);
    profiles_.try_emplace(name, profile);
}

bool Settings::remove(const std::u8string &profile)
{
    if (!get(profile).profileDirectory().removeRecursively())
        return false;

    update(true);

    if (commonSettings().profile == profile)
    {
        commonSettings().profile = defaultProfile;
        saveCommonSettings();
    }

    return true;
}

bool Settings::rename(const std::u8string &profile, const std::u8string &newName)
{
    if (!rootProfileDir_.rename(profile, newName))
        return false;

    profiles_.erase(profile);
    update();

    if (commonSettings().profile == profile)
    {
        commonSettings().profile = newName;
        saveCommonSettings();
    }

    return true;
}

Profile &Settings::setCurrent(const std::u8string &profile)
{
    const std::u8string &target = exists(profile) ? profile : defaultProfile;

    if (!profiles_.count(target))
        throw std::runtime_error(
            "Trying to load a profile that does not exist. Please reinstall the application");

    commonSettings_.profile = target;
    return profiles_.at(target);
}

Profile &Settings::getCurrent()
{
    if (!exists(currentProfileName()))
        return setCurrent(defaultProfile);

    return profiles_.at(currentProfileName());
}

std::u8string Settings::currentProfileName()
{
    return commonSettings_.profile;
}

Profile &Settings::get(const std::u8string &profile)
{
    return profiles_.at(profile);
}

bool Settings::exists(const std::u8string &profile)
{
    return list().contains(profile) && !profile.isEmpty();
}

std::u8stringList Settings::list()
{
    std::u8stringList list;
    for (const auto &[key, value] : profiles_)
        list.push_back(key);
    return list;
}

void Settings::update(bool fullRefresh)
{
    if (fullRefresh)
        profiles_.clear();

    const auto &dirList = rootProfileDir_.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const auto &subDir : dirList)
    {
        const std::u8string &profilePath = rootProfileDir_.absoluteFilePath(subDir);
        const std::u8string &settingPath = profilePath + '/' + Profile::generalSettingsFileName;
        if (QFile::exists(settingPath) && !profiles_.count(subDir))
        {
            profiles_.try_emplace(subDir, Profile(profilePath));
        }
    }
}

std::u8string Settings::commonSettingsPath()
{
    return rootProfileDir_.filePath(commonSettingsFileName);
}

void Settings::saveCommonSettings()
{
    nlohmann::json j = commonSettings_.getJSON();
    JSON::saveToFile(j, commonSettingsPath());
}

void Settings::beginRun()
{
    for (auto &func : callOnceList())
        func();
}

void Settings::callWhenRunStart(const std::function<void()> &callable)
{
    callOnceList().emplace_back(callable);
}

std::vector<std::function<void()>> &Settings::callOnceList()
{
    static std::vector<std::function<void()>> list_;
    return list_;
}
  FIXME */

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
