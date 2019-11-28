/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Settings/Profiles.hpp"
#include "Manager.hpp"

namespace CAO {
const QString defaultProfile = "SSE";

Profiles *Profiles::_instance;

Profiles::Profiles(){};

void Profiles::init()
{
    _commonSettings = new QSettings("profiles/common.ini", QSettings::IniFormat, this);
    findProfiles(QDir("profiles"));
    const QString &mode = _commonSettings->value("profile").toString();
    loadProfile(mode);
}

size_t Profiles::findProfiles(const QDir &dir)
{
    _profileDir = dir;
    size_t counter = 0;
    _profiles.clear();
    for (const auto &subDir : _profileDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        const QString &ini = dir.absoluteFilePath(subDir + "/Settings.json");
        if (QFile::exists(ini))
        {
            _profiles << subDir;
            ++counter;
        }
    }
    PLOG_VERBOSE << "Profiles found: " << _profiles.join('\n');
    return counter;
}

void Profiles::loadProfile(const QString &newProfile)
{
    _currentProfile = exists(newProfile) ? newProfile : defaultProfile;
    _commonSettings->setValue("profile", _currentProfile);

    _logPath = QDir::toNativeSeparators(
        QString("%1/logs/%2/%3.html")
            .arg(QDir::currentPath(), _currentProfile, QDateTime::currentDateTime().toString("yy.MM.dd.hh.mm")));
    const QString &folder = _profileDir.absoluteFilePath(_currentProfile);

    _isBaseProfile = QFile::exists(folder + "/isBase");
    _settingsPath = folder + "/Settings.json";
}

bool Profiles::exists(const QString &profile)
{
    getInstance().findProfiles(QDir("profiles"));
    return getInstance()._profiles.contains(profile) && !profile.isEmpty();
}

QStringList Profiles::list()
{
    return getInstance()._profiles;
}

void Profiles::create(const QString &name, const QString &baseProfile)
{
    const QString &baseFolder = getInstance()._profileDir.absoluteFilePath(exists(baseProfile) ? baseProfile
                                                                                               : defaultProfile);
    const QString &newFolder = getInstance()._profileDir.absoluteFilePath(name);
    FilesystemOperations::copyDir(baseFolder, newFolder, false);
    QFile::remove(newFolder + "/isBase");
    getInstance().findProfiles(getInstance()._profileDir);
}

QFile Profiles::getFile(const QString &filename)
{
    const QDir &currentProfileDir(getInstance()._profileDir.absoluteFilePath(getInstance().currentProfile()));
    const QDir &defaultProfileDir(getInstance()._profileDir.absoluteFilePath(defaultProfile));

    if (currentProfileDir.exists(filename))
        return QFile(currentProfileDir.filePath(filename));
    else if (defaultProfileDir.exists(filename))
        return QFile(defaultProfileDir.filePath(filename));
    else
        return QFile();
}

Profiles &Profiles::getInstance()
{
    if (!_instance)
    {
        _instance = new Profiles();
        _instance->init();
    }

    return *_instance;
}

} // namespace CAO
