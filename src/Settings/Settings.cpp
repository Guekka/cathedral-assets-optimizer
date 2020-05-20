/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Settings.hpp"
#include "Settings/Profiles.hpp"

namespace CAO {

Settings::Settings(const nlohmann::json &j)
    : json_(nlohmann::json(j))
{
}

Settings::Settings(const Settings &other)
    : json_(other.json_)
{
}

Settings::Settings(Settings &&other)
    : json_(std::move(other.json_))
{
}

void Settings::operator=(const Settings &other)
{
    json_ = other.json_;
}

void Settings::saveToUi(MainWindow &window) const
{
    for (const auto &set : uiSyncList_)
        set.save(window, json_);
}

void Settings::readFromUi(const MainWindow &window)
{
    for (const auto &set : uiSyncList_)
        set.read(window, json_);
}

PatternSettings::PatternSettings()
    : patterns_{"*"}
{
}

PatternSettings::PatternSettings(const nlohmann::json &json)
    : Settings(json)
{
    setJSON(json);
}

PatternSettings::PatternSettings(size_t priority, const std::vector<std::string> &wildcards)
    : patterns_(wildcards)
    , priority_(priority)
{
}

PatternSettings::PatternSettings(const PatternSettings &other)
    : Settings(other.json_)
    , patterns_(other.patterns_)
    , priority_(other.priority_)
{
}

PatternSettings::PatternSettings(PatternSettings &&other)
    : Settings(std::move(other.json_))
    , patterns_(std::move(other.patterns_))
    , priority_(std::move(other.priority_))
{
}

void PatternSettings::operator=(const PatternSettings &other)
{
    json_     = other.json_;
    patterns_ = other.patterns_;
    priority_ = other.priority_;
}

bool PatternSettings::operator==(const PatternSettings &other) const
{
    return getJSON() == other.getJSON();
}

nlohmann::json PatternSettings::getJSON() const
{
    nlohmann::json j = json_;
    JSON::setValue(j, priorityKey, priority_);
    JSON::setValue(j, patternKey, patterns_);
    return j;
}

nlohmann::json PatternSettings::getJSONWithoutMeta() const
{
    return json_;
}

void PatternSettings::setJSON(const nlohmann::json &j)
{
    json_          = j;
    patterns_      = getPatternWildcardsFromJSON(json_);
    auto oPriority = getPatternPriorityFromJSON(json_);
    priority_      = oPriority ? *oPriority : 0;
    json_.erase(patternKey);
    json_.erase(priorityKey);
}

std::vector<std::string> PatternSettings::getPatternWildcardsFromJSON(const nlohmann::json &json)
{
    if (json.contains(PatternSettings::patternKey))
        return JSON::getValue<std::vector<std::string>>(json, PatternSettings::patternKey);
    else
        return {};
}

std::optional<size_t> PatternSettings::getPatternPriorityFromJSON(const nlohmann::json &json)
{
    if (!json.contains(PatternSettings::priorityKey) || !json[PatternSettings::priorityKey].is_number())
        return std::nullopt;

    return json[PatternSettings::priorityKey].get<size_t>();
}

std::optional<QString> PatternSettings::isValid()
{
    if (iMeshesOptimizationLevel() > 3)
        return ("This meshes optimization level does not exist. Level: "
                + QString::number(iMeshesOptimizationLevel()));

    if (iTexturesResizingBySizeWidth() % 2 != 0 || iTexturesResizingBySizeHeight() % 2 != 0)
        return ("Textures resizing target size has to be a power of two");

    if (iTexturesResizingByRatioWidth() % 2 != 0 || iTexturesResizingByRatioHeight() % 2 != 0)
        return ("Textures resizing ratio has to be a power of two");

    return std::nullopt;
}

GeneralSettings::GeneralSettings(nlohmann::json j)
    : Settings(j)
{
}

GeneralSettings::GeneralSettings(const GeneralSettings &other)
    : Settings(other.json_)
{
}

GeneralSettings::GeneralSettings(GeneralSettings &&other)
    : Settings(std::move(other))
{
}

void GeneralSettings::operator=(const GeneralSettings &other)
{
    json_ = other.json_;
}

QString GeneralSettings::isValid() const
{
    if (!QDir(sUserPath()).exists() || sUserPath().size() < 5)
    {
        return QString("This path does not exist or is shorter than 5 characters. Path: '%1'").arg(sUserPath());
    }

    return QString();
}

} // namespace CAO
