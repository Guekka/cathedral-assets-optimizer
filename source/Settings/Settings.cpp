/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Settings.hpp"
#include "Settings/Profiles.hpp"

namespace CAO {

PatternSettings::PatternSettings(const nlohmann::json &json)
    : Settings(json)
{
    setJSON(json_);
}

PatternSettings::PatternSettings(size_t priority, const std::vector<QRegularExpression> &regex)
    : regexes_(regex)
    , priority_(priority)
{}

nlohmann::json PatternSettings::getJSON() const
{
    nlohmann::json j = json_;
    JSON::setValue(j, priorityKey, priority_);
    JSON::setValue(j, regexKey, toStringList(regexes_));
    return j;
}

void PatternSettings::setJSON(const nlohmann::json &j)
{
    json_ = j;
    regexes_ = getPatternRegexFromJSON(json_);
    auto oPriority = getPatternPriorityFromJSON(json_);
    priority_ = oPriority ? *oPriority : 0;
    json_.erase(patternKey);
    json_.erase(regexKey);
    json_.erase(priorityKey);
}

std::vector<QRegularExpression> PatternSettings::getPatternRegexFromJSON(const nlohmann::json &json)
{
    QStringList regexStrings;
    bool wildcards = false;
    if (json.contains(PatternSettings::patternKey)) {
        regexStrings = JSON::getValue<QStringList>(json, PatternSettings::patternKey);
        wildcards = true;
    }
    if (json.contains(PatternSettings::regexKey))
        regexStrings = JSON::getValue<QStringList>(json, PatternSettings::regexKey);
    else if (!json.contains(PatternSettings::patternKey)
             && !json.contains(PatternSettings::regexKey))
        return std::vector<QRegularExpression>{};

    return toRegexVector(regexStrings, wildcards);
}

std::optional<size_t> PatternSettings::getPatternPriorityFromJSON(const nlohmann::json &json)
{
    if (!json.contains(PatternSettings::priorityKey)
        || !json[PatternSettings::priorityKey].is_number())
        return std::nullopt;

    return json[PatternSettings::priorityKey].get<size_t>();
}

std::optional<QString> PatternSettings::isValid()
{
    if (iMeshesOptimizationLevel() > 3)
        return ("This meshes optimization level does not exist. Level: "
                + QString::number(iMeshesOptimizationLevel()));

    if (iTexturesTargetWidth() % 2 != 0 || iTexturesTargetHeight() % 2 != 0)
        return ("Textures target size has to be a power of two");

    return std::nullopt;
}

GeneralSettings::GeneralSettings(nlohmann::json j)
    : Settings(j)
{
}

QString GeneralSettings::isValid() const
{
    if (!QDir(sUserPath()).exists() || sUserPath().size() < 5) {
        return QString("This path does not exist or is shorter than 5 characters. Path: '%1'")
            .arg(sUserPath());
    }

    return QString();
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

} // namespace CAO