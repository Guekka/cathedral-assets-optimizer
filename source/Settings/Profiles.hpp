/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"
#ifdef GUI
#include "ui_mainWindow.h"
#endif

namespace CAO {
constexpr double GigaByte = 1024 * 1024 * 1024;

class Profiles final : public QObject
{
public:
    [[nodiscard]] static bool exists(const QString &profile);
    void loadProfile(const QString &newProfile);
    [[nodiscard]] static QStringList list();
    static void create(const QString &name, const QString &baseProfile);
    [[nodiscard]] static QFile getFile(const QString &filename);

    //static getters
    [[nodiscard]] static bool isBaseProfile() { return getInstance()._isBaseProfile; }

    [[nodiscard]] static QString logPath() { return getInstance()._logPath; }
    [[nodiscard]] static QString settingsPath() { return getInstance()._settingsPath; }

    [[nodiscard]] static QSettings *commonSettings() { return getInstance()._commonSettings; }

    [[nodiscard]] static Profiles &getInstance();
    [[nodiscard]] static QString currentProfile() { return getInstance()._currentProfile; }

    //static setter
    static void setCurrentProfile(const QString &newProfile) { getInstance().loadProfile(newProfile); }

private:
    size_t findProfiles(const QDir &dir);

    QString _logPath;
    QString _settingsPath;

    QDir _profileDir;
    QString _currentProfile;
    QStringList _profiles;

    QSettings *_commonSettings;

    bool _isBaseProfile;

    static Profiles *_instance;

    Profiles();
    void init();
};
} // namespace CAO
