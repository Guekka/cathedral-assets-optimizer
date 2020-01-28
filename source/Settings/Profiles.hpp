/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings/PatternMap.hpp"

#include "pch.hpp"
class MainWindow;

//Required to use QString as key to map
namespace std {
template<>
struct hash<QString>
{
    std::size_t operator()(const QString &s) const noexcept
    {
        return static_cast<size_t>(qHash(s));
    }
};
} // namespace std

namespace CAO {

class Profile final
{
public:
    Profile(QDir profileDir);

    QFile getFile(const QString &filename) const;

    /* Getters */
    bool isBaseProfile() const;
    QDir profileDirectory() const;
    QString logPath() const;
    QString settingsPath() const;

    /* Settings */
    PatternSettings &getSettings(const QString &filePath);
    const PatternSettings &getSettings(const QString &filePath) const;
    GeneralSettings &getGeneralSettings();
    const GeneralSettings &getGeneralSettings() const;

    void saveToJSON();

    void saveToUi(MainWindow &window);
    void readFromUi(const MainWindow &window);

private:
    GeneralSettings generalSettings_;
    PatternMap patternSettings_;

    QDir profileDir_;
    QString logPath_;
};

class Profiles
{
public:
    /* Constructor */
    Profiles();

    /* Profiles operations */
    void create(const QString &name, const QString &baseProfile);
    //Also sets the current profile. Reads the current profile from INI
    Profile &setCurrent(const QString &profile);
    //!Current profile
    Profile &getCurrent();

    QString currentProfileName();

    Profile &get(const QString &profile);

    QStringList list();
    bool exists(const QString &profile);

    void update(bool fullRefresh = false);

    QSettings &commonSettings() { return _commonSettings; }

private:
    std::unordered_map<QString, Profile> profiles_;
    QDir rootProfileDir_;
    QSettings _commonSettings;
};
} // namespace CAO
