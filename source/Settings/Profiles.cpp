/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Settings/Profiles.hpp"
#include "Manager.hpp"
#include <filesystem>

namespace CAO {
const QString defaultProfile = "SSE";
QString Profiles::_currentProfile;

Profiles::Profiles()
{
    _commonSettings = new QSettings("profiles/common.ini", QSettings::IniFormat, this);
    findProfiles(QDir("profiles"));
    const QString &profile = _commonSettings->value("profile").toString();
    loadProfile(profile);
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
    listPatterns();
}

bool Profiles::exists(const QString &profile)
{
    findProfiles(QDir("profiles"));
    return _profiles.contains(profile) && !profile.isEmpty();
}

QStringList Profiles::list()
{
    return _profiles;
}

void Profiles::create(const QString &name, const QString &baseProfile)
{
    const QString &baseFolder = _profileDir.absoluteFilePath(exists(baseProfile) ? baseProfile : defaultProfile);
    const QString &newFolder = _profileDir.absoluteFilePath(name);
    FilesystemOperations::copyDir(baseFolder, newFolder, false);
    QFile::remove(newFolder + "/isBase");
    findProfiles(_profileDir);
}

QFile Profiles::getFile(const QString &filename)
{
    const QDir &currentProfileDir(_profileDir.absoluteFilePath(currentProfile()));
    const QDir &defaultProfileDir(_profileDir.absoluteFilePath(defaultProfile));

    if (currentProfileDir.exists(filename))
        return QFile(currentProfileDir.filePath(filename));
    else if (defaultProfileDir.exists(filename))
        return QFile(defaultProfileDir.filePath(filename));
    else
        return QFile();
}

void Profiles::listPatterns()
{
    JSON customJson;
    customJson.readFromJSON(_settingsPath);
    auto json = customJson.getInternalJSON().at("Patterns");
    settingsPatterns_.clear();

    //Retrieving all the patterns and converting them to regexes
    for (auto it = json.begin(); it != json.end(); ++it)
    {
        const int priority = (*it).value<int>("Priority", 0);
        QString regexString;
        if (it->contains("Pattern"))
        {
            const QString &patternString = QString::fromStdString(it->at("Pattern").get<std::string>());
            regexString = QRegularExpression::wildcardToRegularExpression(patternString);
        }
        else if (it->contains("Regex"))
            regexString = QString::fromStdString(it->at("Regex").get<std::string>());
        else
            continue; //Current JSON is not valid

        //We only know that the first profile will contain all the settings. The others might be incomplete
        if (!settingsPatterns_.empty())
        {
            const Settings &defaultSets = settingsPatterns_.begin()->second.second;
            const auto &defaultJson = defaultSets.getJSON().getInternalJSON();
            auto patchedJson = defaultJson;
            patchedJson.merge_patch(*it);
            *it = patchedJson;
        }

        QRegularExpression regex(regexString);
        regex.optimize();
        const std::pair<int, std::pair<QRegularExpression, Settings>> pair(priority, {regex, Settings(*it)});
        settingsPatterns_.insert(pair);
    }
}

const Settings &Profiles::getSettings(const QString &filePath) const
{
    const Settings *result = &settingsPatterns_.begin()->second.second;
    for (auto it = settingsPatterns_.cend(); it != settingsPatterns_.cbegin(); --it)
    {
        const auto &regex = it->second.first;
        const auto &match = regex.match(filePath);
        if (match.hasMatch())
        {
            result = &it->second.second;
            break;
        }
    }

    return *result;
}

Settings &Profiles::getDefaultSettings()
{
    if (settingsPatterns_.empty())
        listPatterns();

    return settingsPatterns_.begin()->second.second;
}

void Profiles::saveToJSON(const QString &filepath) const
{
    nlohmann::json json;
    std::vector<nlohmann::json> vector;
    vector.reserve(settingsPatterns_.size());

    std::transform(settingsPatterns_.cbegin(),
                   settingsPatterns_.cend(),
                   std::back_inserter(vector),
                   [](const auto &pair) { return pair.second.second.getJSON().getInternalJSON(); });

    json["Patterns"] = vector;
    std::fstream(std::filesystem::u8path(filepath.toStdString()), std::fstream::out) << std::setw(4) << json;
}

#ifdef GUI
void Profiles::readFromUi(Ui::MainWindow &ui, Ui::BSAFilesToPack &bsUi)
{
    for (const Setting *set : settingsList)
        set->readFromUI(ui, bsUi, getDefaultSettings().getJSON());
}

void Profiles::saveToUi(Ui::MainWindow &ui, Ui::BSAFilesToPack &bsUi)
{
    for (const Setting *set : settingsList)
        set->saveToUI(ui, bsUi, getDefaultSettings().getJSON());
}
#endif

} // namespace CAO
