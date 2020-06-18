/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Settings/Profiles.hpp"
#include "Manager.hpp"

namespace CAO {

Profiles::Profiles()
    : Profiles(QDir("profiles"))
{
}

Profiles::Profiles(QDir dir)
    : rootProfileDir_(std::move(dir))
{
    update(true);
    nlohmann::json j;
    JSON::readFromFile(j, commonSettingsPath());
    commonSettings_.setJSON(j);
}

void Profiles::setDir(const QDir &dir)
{
    rootProfileDir_ = dir;
    profiles_.clear();
}

void Profiles::create(const QString &name, const QString &baseProfile)
{
    const QString &baseFolder = rootProfileDir_.absoluteFilePath(
        exists(baseProfile) ? baseProfile : defaultProfile);
    const QString &newFolder = rootProfileDir_.absoluteFilePath(name);
    Filesystem::copyDir(baseFolder, newFolder, false);
    QFile::remove(newFolder + "/isBase");
    profiles_.try_emplace(name, Profile(newFolder));
}

void Profiles::create(const QString &name)
{
    const QString &newFolder = rootProfileDir_.absoluteFilePath(name);
    rootProfileDir_.mkpath(newFolder);
    profiles_.try_emplace(name, Profile(newFolder));
}

void Profiles::create(const Profile &profile, const QString &name)
{
    const QString &newFolder = rootProfileDir_.absoluteFilePath(name);
    rootProfileDir_.mkpath(newFolder);
    profiles_.try_emplace(name, profile);
}

Profile &Profiles::setCurrent(const QString &profile)
{
    if (!profiles_.count(profile))
        throw std::runtime_error(
            "Trying to load a profile that does not exist. Please reinstall the application");

    commonSettings_.sProfile = profile;
    return profiles_.at(profile);
}
Profile &Profiles::getCurrent()
{
    //Current profile
    const QString &profile = currentProfileName(); //Safety check
    return setCurrent(profile.isEmpty() ? defaultProfile : profile);
}

QString Profiles::currentProfileName()
{
    return commonSettings_.sProfile();
}

Profile &Profiles::get(const QString &profile)
{
    return profiles_.at(profile);
}

bool Profiles::exists(const QString &profile)
{
    return list().contains(profile) && !profile.isEmpty();
}

QStringList Profiles::list()
{
    QStringList list;
    for (const auto &[key, value] : profiles_)
        list.push_back(key);
    return list;
}

void Profiles::update(bool fullRefresh)
{
    if (fullRefresh)
        profiles_.clear();

    const auto &dirList = rootProfileDir_.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const auto &subDir : dirList) {
        const QString &profilePath = rootProfileDir_.absoluteFilePath(subDir);
        const QString &settingPath = profilePath + '/' + Profile::generalSettingsFileName;
        if (QFile::exists(settingPath) && !profiles_.count(subDir)) {
            profiles_.try_emplace(subDir, Profile(profilePath));
        }
    }
}

QString Profiles::commonSettingsPath()
{
    return rootProfileDir_.filePath(commonSettingsFileName);
}

void Profiles::saveCommonSettings()
{
    nlohmann::json j = commonSettings_.getJSON();
    JSON::saveToFile(j, commonSettingsPath());
}
} // namespace CAO
