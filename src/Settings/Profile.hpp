/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings/FileTypes.hpp"
#include "Settings/GeneralSettings.hpp"
#include "Settings/PatternMap.hpp"
#include "pch.hpp"

namespace CAO {
class Profile
{
public:
    static inline const QString generalSettingsFileName = "GeneralSettings.json";
    static inline const QString patternSettingsFileName = "PatternSettings.json";
    static inline const QString fileTypesFilename       = "FileTypes.json";
    static inline const QString isBaseProfileFilename   = "isBase";

    Profile(QDir profileDir);

    QFile getFile(const QString &filename) const;

    /* Getters */
    bool isBaseProfile() const;
    QDir profileDirectory() const;

    QString logPath() const;
    QString generalSettingsPath() const;
    QString patternSettingsPath() const;
    QString fileTypesPath() const;

    /* Settings */
    PatternSettings getSettings(const QString &filePath) const;

    GeneralSettings &getGeneralSettings();
    const GeneralSettings &getGeneralSettings() const;

    PatternMap &getPatterns();
    const PatternMap &getPatterns() const;

    FileTypes &getFileTypes();
    const FileTypes &getFileTypes() const;

    void saveToJSON();

private:
    FileTypes fileTypes_;
    GeneralSettings generalSettings_;
    PatternMap patternSettings_;

    QDir profileDir_;
    QString logPath_;
};
} // namespace CAO
