/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Settings/Profiles.hpp"
#include "Manager.hpp"

namespace CAO {

Profile::Profile(QDir profileDir)
    : profileDir_(std::move(profileDir))
{
    //Log path
    const QString &dateTime = QDateTime::currentDateTime().toString("yy.MM.dd.hh.mm");
    const QString &absolutePath = profileDir_.absoluteFilePath("logs/" + dateTime + ".html");
    logPath_ = QDir::toNativeSeparators(absolutePath);

    nlohmann::json generalJson;
    JSON::readFromFile(generalJson, generalSettingsPath());
    generalSettings_ = {generalJson};

    nlohmann::json patternJson;
    JSON::readFromFile(patternJson, patternSettingsPath());
    patternSettings_.listPatterns(patternJson);
}

QFile Profile::getFile(const QString &filename) const
{
    if (!profileDir_.exists(filename))
        return QFile();
    return QFile(profileDir_.absoluteFilePath(filename));
}

bool Profile::isBaseProfile() const
{
    return profileDir_.exists(isBaseProfileFilename);
}

QDir Profile::profileDirectory() const
{
    return profileDir_;
}

QString Profile::logPath() const
{
    return logPath_;
}

QString Profile::generalSettingsPath() const
{
    return profileDir_.absoluteFilePath(generalSettingsFileName);
}

QString Profile::patternSettingsPath() const
{
    return profileDir_.absoluteFilePath(patternSettingsFileName);
}

PatternSettings &Profile::getSettings(const QString &filePath)
{
    return patternSettings_.getSettings(filePath);
}

const PatternSettings &Profile::getSettings(const QString &filePath) const
{
    return patternSettings_.getSettings(filePath);
}

GeneralSettings &Profile::getGeneralSettings()
{
    return generalSettings_;
}

const GeneralSettings &Profile::getGeneralSettings() const
{
    return generalSettings_;
}

PatternMap &Profile::getPatterns()
{
    return patternSettings_;
}

const PatternMap &Profile::getPatterns() const
{
    return patternSettings_;
}

void Profile::saveToJSON()
{
    nlohmann::json generalJson = getGeneralSettings().getJSON();
    nlohmann::json patternJSON = patternSettings_.getUnifiedJSON();
    JSON::saveToFile(generalJson, generalSettingsPath());
    JSON::saveToFile(patternJSON, patternSettingsPath());
}

#ifdef GUI
void Profile::readFromUi(const MainWindow &window)
{
    generalSettings_.readFromUi(window);
    patternSettings_.readFromUi(window);
}

void Profile::saveToUi(MainWindow &window) const
{
    generalSettings_.saveToUi(window);
    patternSettings_.saveToUi(window);
}
#endif

/* Profiles class */

Profiles::Profiles()
    : rootProfileDir_("profiles")
    , _commonSettings(rootProfileDir_.filePath(commonSettingsFileName), QSettings::IniFormat)
{
    update(true);
}

Profiles &Profiles::getInstance()
{
    static Profiles instance_;
    return instance_;
}

void Profiles::setDir(const QDir &dir)
{
    rootProfileDir_ = dir;
}

void Profiles::create(const QString &name, const QString &baseProfile)
{
    const QString &baseFolder = rootProfileDir_.absoluteFilePath(
        exists(baseProfile) ? baseProfile : defaultProfile);
    const QString &newFolder = rootProfileDir_.absoluteFilePath(name);
    Filesystem::copyDir(baseFolder, newFolder, false);
    QFile::remove(newFolder + "/isBase");
    profiles_.emplace(name, Profile(newFolder));
}

void Profiles::create(const QString &name)
{
    const QString &newFolder = rootProfileDir_.absoluteFilePath(name);
    rootProfileDir_.mkpath(newFolder);
    profiles_.emplace(name, Profile(newFolder));
}

void Profiles::create(const Profile &profile, const QString &name)
{
    const QString &newFolder = rootProfileDir_.absoluteFilePath(name);
    rootProfileDir_.mkpath(newFolder);
    profiles_.emplace(name, profile);
}

Profile &Profiles::setCurrent(const QString &profile)
{
    if (!profiles_.count(profile))
        throw std::runtime_error(
            "Trying to load a profile that does not exist. Please reinstall the application");

    _commonSettings.setValue("profile", profile);
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
    return _commonSettings.value("profile").toString();
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
            profiles_.emplace(subDir, Profile(profilePath));
        }
    }
}
} // namespace CAO
