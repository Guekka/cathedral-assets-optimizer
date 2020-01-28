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
    //TODO might want to separate patterns and general settings into different files

    //Log path
    const QString &dateTime = QDateTime::currentDateTime().toString("yy.MM.dd.hh.mm");
    const QString &absolutePath = profileDir_.absoluteFilePath("logs/" + dateTime + ".html");
    logPath_ = QDir::toNativeSeparators(absolutePath);

    JSON generalJson;
    const QString &filepath = profileDir_.absoluteFilePath("Settings.json");
    generalJson.readFromJSON(filepath);

    nlohmann::json j = generalJson.getInternalJSON();
    generalSettings_ = {j};

    patternSettings_.listPatterns(j["Patterns"]);
}

QFile Profile::getFile(const QString &filename) const
{
    if (!profileDir_.exists(filename))
        return QFile();
    return QFile(profileDir_.absoluteFilePath(filename));
}

bool Profile::isBaseProfile() const
{
    return profileDir_.exists("isBase");
}

QDir Profile::profileDirectory() const
{
    return profileDir_;
}

QString Profile::logPath() const
{
    return logPath_;
}

QString Profile::settingsPath() const
{
    return profileDir_.absoluteFilePath("Settings.json");
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

void Profile::saveToJSON()
{
    nlohmann::json json = getGeneralSettings().getJSON().getInternalJSON();
    json["Patterns"] = patternSettings_.getUnifiedJSON();
    JSON(json).saveToJSON(settingsPath());
}

#ifdef GUI
void Profile::readFromUi(const MainWindow &window)
{
    generalSettings_.readFromUi(window);
    patternSettings_.readFromUi(window);
}

void Profile::saveToUi(MainWindow &window)
{
    generalSettings_.saveToUi(window);
    patternSettings_.saveToUi(window);
}
#endif

/* Profiles class */
const QString defaultProfile = "SSE";

Profiles::Profiles()
    : rootProfileDir_("profiles")
    , _commonSettings(rootProfileDir_.filePath("common.ini"), QSettings::IniFormat)
{
    update(true);
}

void Profiles::create(const QString &name, const QString &baseProfile)
{
    const QString &baseFolder = rootProfileDir_.absoluteFilePath(
        exists(baseProfile) ? baseProfile : defaultProfile);
    const QString &newFolder = rootProfileDir_.absoluteFilePath(name);
    FilesystemOperations::copyDir(baseFolder, newFolder, false);
    QFile::remove(newFolder + "/isBase");
    update();
}

Profile &Profiles::setCurrent(const QString &profile)
{
    if (!profiles_.count(profile))
        throw std::runtime_error("Trying to load a profile that does not exist.");

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
        const QString &settings = rootProfileDir_.absoluteFilePath(subDir + "/Settings.json");
        if (QFile::exists(settings))
            if (!profiles_.count(subDir))
                profiles_.emplace(subDir, Profile(subDir));
    }
}
} // namespace CAO
