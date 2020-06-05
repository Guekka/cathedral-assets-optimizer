/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "PatternSettings.hpp"

namespace CAO {
PatternSettings::PatternSettings()
    : patterns_{"*"}
{
}

PatternSettings::PatternSettings(const nlohmann::json &json)
{
    setJSON(json);
}

PatternSettings::PatternSettings(size_t priority, const std::vector<std::string> &wildcards)
    : patterns_(wildcards)
    , priority_(priority)
{
}

PatternSettings::PatternSettings(const PatternSettings &other)
    : patterns_(other.patterns_)
    , priority_(other.priority_)
{
    json_ = other.json_;
}

PatternSettings::PatternSettings(PatternSettings &&other)
    : patterns_(std::move(other.patterns_))
    , priority_(std::move(other.priority_))
{
    json_ = std::move(other.json_);
}

PatternSettings &PatternSettings::operator=(const PatternSettings &other)
{
    if (this == &other)
        return *this;

    json_     = other.json_;
    patterns_ = other.patterns_;
    priority_ = other.priority_;

    return *this;
}

PatternSettings &PatternSettings::operator=(PatternSettings &&other)
{
    if (this == &other)
        return *this;

    json_     = std::move(other.json_);
    patterns_ = std::move(other.patterns_);
    priority_ = std::move(other.priority_);

    return *this;
}

bool PatternSettings::operator==(const PatternSettings &other) const
{
    return getJSON() == other.getJSON();
}

std::optional<QString> PatternSettings::isValid() const
{
    auto isPow2 = [](int val) { return val % 2 == 0; };

    if (!isPow2(iTexturesMinimumWidth()) || !isPow2(iTexturesMinimumHeight()))
        return ("Textures resizing minimum size has to be a power of two");

    if (iTexturesResizingBySizeWidth() % 2 != 0 || iTexturesResizingBySizeHeight() % 2 != 0)
        return ("Textures resizing target size has to be a power of two");

    if (iTexturesResizingByRatioWidth() % 2 != 0 || iTexturesResizingByRatioHeight() % 2 != 0)
        return ("Textures resizing ratio has to be a power of two");

    if (iMeshesOptimizationLevel() < 0 || iMeshesOptimizationLevel() > 3)
        return ("This meshes optimization level does not exist. Level: "
                + QString::number(iMeshesOptimizationLevel()));

    return std::nullopt;
}

nlohmann::json PatternSettings::getJSON() const
{
    nlohmann::json j = json_;
    JSON::setValue(j, priorityKey, priority_);
    JSON::setValue(j, patternKey, patterns_);
    return j;
}

nlohmann::json PatternSettings::removeMeta(const nlohmann::json &j)
{
    auto result = j;
    result.erase(priorityKey);
    result.erase(patternKey);
    return result;
}

void PatternSettings::setJSON(const nlohmann::json &j)
{
    priority_ = getPatternPriorityFromJSON(j).value_or(0);
    patterns_ = getPatternWildcardsFromJSON(j);
    json_     = removeMeta(j);
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

} // namespace CAO
