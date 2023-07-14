/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "JSON.hpp"
#include "per_file_settings.hpp"

namespace cao {
/*
PerFileSettings::PerFileSettings()
    : pattern{"*"}
{
}

PerFileSettings::PerFileSettings(const nlohmann::json &json)
{
    setJSON(json);
}

PerFileSettings::PerFileSettings(size_t priority, const std::string &wildcard)
    : pattern(wildcard)
    , priority(priority)
{
}

PerFileSettings::PerFileSettings(const PerFileSettings &other)
    : pattern(other.pattern)
    , priority(other.priority)
{
    json_ = other.json_;
}

PerFileSettings::PerFileSettings(PerFileSettings &&other) noexcept
    : pattern(std::move(other.pattern))
    , priority(std::move(other.priority))
{
    json_ = std::move(other.json_);
}

PerFileSettings &PerFileSettings::operator=(const PerFileSettings &other)
{
    if (this == &other)
        return *this;

    json_    = other.json_;
    pattern  = other.pattern;
    priority = other.priority;

    return *this;
}

PerFileSettings &PerFileSettings::operator=(PerFileSettings &&other) noexcept
{
    if (this == &other)
        return *this;

    json_    = std::move(other.json_);
    pattern  = std::move(other.pattern);
    priority = std::move(other.priority);

    return *this;
}

bool PerFileSettings::operator==(const PerFileSettings &other) const
{
    return getJSON() == other.getJSON();
}

std::optional<QString> PerFileSettings::isValid() const
{
    auto isPow2 = [](int val) { return val % 2 == 0; };

    if (!isPow2(iTexturesMinimumWidth()) || !isPow2(iTexturesMinimumHeight()))
        return tr("Textures resizing minimum size has to be a power of two");

    if (iTexturesResizingWidth() % 2 != 0 || iTexturesResizingHeight() % 2 != 0)
        return tr("Textures resizing target has to be a power of two");

    if (iMeshesOptimizationLevel() < 0 || iMeshesOptimizationLevel() > 2)
        return tr("This meshes optimization level does not exist. Level: %1")
            .arg(QString::number(iMeshesOptimizationLevel()));

    return std::nullopt;
}

nlohmann::json PerFileSettings::getJSON() const
{
    nlohmann::json j = json_;
    JSON::setValue(j, priorityKey, priority);
    JSON::setValue(j, patternKey, pattern);
    return j;
}

nlohmann::json PerFileSettings::removeMeta(const nlohmann::json &j)
{
    auto result = j;
    if (result.contains(priorityKey))
        result.erase(priorityKey);
    if (result.contains(patternKey))
        result.erase(patternKey);
    return result;
}

void PerFileSettings::setJSON(const nlohmann::json &j)
{
    priority = getPatternPriorityFromJSON(j).value_or(0);
    pattern  = getPatternWildcardsFromJSON(j);
    json_    = removeMeta(j);
}

std::string PerFileSettings::getPatternWildcardsFromJSON(const nlohmann::json &json)
{
    if (json.contains(PerFileSettings::patternKey))
        return JSON::getValue<std::string>(json, PerFileSettings::patternKey);
    else
        return {};
}

std::optional<size_t> PerFileSettings::getPatternPriorityFromJSON(const nlohmann::json &json)
{
    if (!json.contains(PerFileSettings::priorityKey) || !json[PerFileSettings::priorityKey].is_number())
        return std::nullopt;

    return json[PerFileSettings::priorityKey].get<size_t>();
}
*/
} // namespace cao
