/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Profile.hpp"
#include "pch.hpp"

class MainWindow;

namespace CAO {

class Profiles
{
public:
    static inline const QString commonSettingsFileName = "common.ini";
    static inline const QString defaultProfile         = "SSE";

    static Profiles &getInstance();

    void setDir(const QDir &dir);

    /* Profiles operations */
    void create(const QString &name, const QString &baseProfile);
    void create(const QString &name);
    void create(const Profile &profile, const QString &name);
    Profile &setCurrent(const QString &profile);

    //Also sets the current profile. Reads the current profile from INI
    Profile &getCurrent();
    Profile &get(const QString &profile);

    QString currentProfileName();

    QStringList list();
    bool exists(const QString &profile);
    void update(bool fullRefresh = false);

    QSettings &commonSettings() { return _commonSettings; }

private:
    std::unordered_map<QString, Profile> profiles_;
    QDir rootProfileDir_;
    QSettings _commonSettings;

    /* Constructor */
    Profiles();
};

static Profile &currentProfile()
{
    return Profiles::getInstance().getCurrent();
}

} // namespace CAO
