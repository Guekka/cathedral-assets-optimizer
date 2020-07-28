/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Profile.hpp"

namespace CAO {
Profile::Profile(QDir profileDir)
    : profileDir_(std::move(profileDir))
{
    //Log path
    const QString &dateTime     = QDateTime::currentDateTime().toString("yy.MM.dd.hh.mm");
    const QString &absolutePath = profileDir_.absoluteFilePath("logs/" + dateTime + ".txt");
    logPath_                    = QDir::toNativeSeparators(absolutePath);

    auto readJSON = [](Settings &sets, const QString &path) {
        nlohmann::json json;
        JSON::readFromFile(json, path);
        sets.setJSON(json);
    };

    readJSON(generalSettings_, generalSettingsPath());
    readJSON(fileTypes_, fileTypesPath());

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

QString Profile::fileTypesPath() const
{
    return profileDir_.absoluteFilePath(fileTypesFilename);
}

PatternSettings Profile::getSettings(const QString &filePath) const
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

FileTypes &Profile::getFileTypes()
{
    return fileTypes_;
}

const FileTypes &Profile::getFileTypes() const
{
    return fileTypes_;
}

void Profile::saveToJSON()
{
    auto saveJSON = [](const Settings &sets, const QString &path) { JSON::saveToFile(sets.getJSON(), path); };

    saveJSON(generalSettings_, generalSettingsPath());
    saveJSON(fileTypes_, fileTypesPath());

    nlohmann::json patternJSON = patternSettings_.getUnifiedJSON();
    JSON::saveToFile(patternJSON, patternSettingsPath());
}
} // namespace CAO
