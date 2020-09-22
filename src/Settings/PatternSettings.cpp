/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "PatternSettings.hpp"

namespace CAO {
PatternSettings::PatternSettings()
    : pattern{"*"}
{
}

PatternSettings::PatternSettings(const nlohmann::json &json)
{
    setJSON(json);
}

PatternSettings::PatternSettings(size_t priority, const std::string &wildcard)
    : pattern(wildcard)
    , priority(priority)
{
}

PatternSettings::PatternSettings(const PatternSettings &other)
    : pattern(other.pattern)
    , priority(other.priority)
{
    json_ = other.json_;
}

PatternSettings::PatternSettings(PatternSettings &&other) noexcept
    : pattern(std::move(other.pattern))
    , priority(std::move(other.priority))
{
    json_ = std::move(other.json_);
}

PatternSettings &PatternSettings::operator=(const PatternSettings &other)
{
    if (this == &other)
        return *this;

    json_    = other.json_;
    pattern  = other.pattern;
    priority = other.priority;

    return *this;
}

PatternSettings &PatternSettings::operator=(PatternSettings &&other) noexcept
{
    if (this == &other)
        return *this;

    json_    = std::move(other.json_);
    pattern  = std::move(other.pattern);
    priority = std::move(other.priority);

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

    if (iTexturesResizingWidth() % 2 != 0 || iTexturesResizingHeight() % 2 != 0)
        return ("Textures resizing target has to be a power of two");

    if (iMeshesOptimizationLevel() < 0 || iMeshesOptimizationLevel() > 3)
        return ("This meshes optimization level does not exist. Level: "
                + QString::number(iMeshesOptimizationLevel()));

    return std::nullopt;
}

nlohmann::json PatternSettings::getJSON() const
{
    nlohmann::json j = json_;
    JSON::setValue(j, priorityKey, priority);
    JSON::setValue(j, patternKey, pattern);
    return j;
}

nlohmann::json PatternSettings::removeMeta(const nlohmann::json &j)
{
    auto result = j;
    if (result.contains(priorityKey))
        result.erase(priorityKey);
    if (result.contains(patternKey))
        result.erase(patternKey);
    return result;
}

void PatternSettings::setJSON(const nlohmann::json &j)
{
    priority = getPatternPriorityFromJSON(j).value_or(0);
    pattern  = getPatternWildcardsFromJSON(j);
    json_    = removeMeta(j);
}

std::string PatternSettings::getPatternWildcardsFromJSON(const nlohmann::json &json)
{
    if (json.contains(PatternSettings::patternKey))
        return JSON::getValue<std::string>(json, PatternSettings::patternKey);
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
