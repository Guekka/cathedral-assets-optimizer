/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings/Settings.hpp"

#include "pch.hpp"
#ifdef GUI
#include "ui_mainWindow.h"
#endif

namespace CAO {
class Profiles final : public QObject
{
public:
    Profiles();

    bool exists(const QString &profile);
    void loadProfile(const QString &newProfile);
    QStringList list();
    void create(const QString &name, const QString &baseProfile);
    QFile getFile(const QString &filename);
    bool isBaseProfile() { return _isBaseProfile; }
    QString logPath() const { return _logPath; }
    QString settingsPath() const { return _settingsPath; }
    QSettings *commonSettings() const { return _commonSettings; }
    static const QString &currentProfile() { return _currentProfile; }
    void setCurrentProfile(const QString &newProfile) { loadProfile(newProfile); }
    const Settings &getSettings(const QString &filePath) const;
    Settings &getDefaultSettings();

    void saveToJSON(const QString &filepath) const;

#ifdef GUI
    void saveToUi(Ui::MainWindow &ui, Ui::BSAFilesToPack &bsUi);
    void readFromUi(Ui::MainWindow &ui, Ui::BSAFilesToPack &bsUi);
#endif

private:
    size_t findProfiles(const QDir &dir);

    QString _logPath;
    QString _settingsPath;

    QDir _profileDir;
    static QString _currentProfile;
    QStringList _profiles;

    QSettings *_commonSettings;

    bool _isBaseProfile;

    std::multimap<int, std::pair<QRegularExpression, Settings>> settingsPatterns_;
    void listPatterns();
};
} // namespace CAO
