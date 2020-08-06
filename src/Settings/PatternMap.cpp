/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "PatternMap.hpp"
#include "Utils/Algorithms.hpp"
#include "Utils/wildcards.hpp"
#include <string>

namespace CAO {
void PatternMap::listPatterns(const nlohmann::json &json)
{
    patterns_.clear();

    for (auto &value : json)
    {
        if (!value.is_object())
            continue;

        addPattern(PatternSettings(value));
    }

    const bool hasDefault = any_of(patterns_, [](auto &&pSets) { return pSets.pattern == "*"; });

    if (!hasDefault)
        addPattern(PatternSettings{0, "*"});
}

void PatternMap::addPattern(PatternSettings pattern)
{
    size_t idx = pattern.priority;
    if (idx > patterns_.size())
    {
        pattern.priority = patterns_.size();
        patterns_.emplace_back(std::move(pattern));
    }
    else
    {
        patterns_.insert(patterns_.begin() + idx, std::move(pattern));
        updatePatternsPriority();
    }
}

PatternSettings PatternMap::getSettings(const QString &filePath) const
{
    assert(!patterns_.empty());

    std::string path = filePath.toStdString();

    auto matchWildcard = [&path](const std::string &wildcard) {
        using namespace wildcards;
        return isMatch(path, pattern{wildcard}, case_insensitive);
    };

    nlohmann::json merged;
    for (const auto &patternSettings : patterns_)
        if (matchWildcard(patternSettings.pattern))
            merged.merge_patch(patternSettings.getJSON());

    return PatternSettings{merged};
}

const PatternSettings &PatternMap::getDefaultSettings() const
{
    return *patterns_.begin();
}

PatternSettings &PatternMap::getDefaultSettings()
{
    return *patterns_.begin();
}

void PatternMap::updatePatternsPriority()
{
    for (size_t i = 0; i < patterns_.size(); i++)
        patterns_[i].priority = i;
}


nlohmann::json PatternMap::getUnifiedJSON() const
{
    auto getJSON = [](const auto &val) { return val.getJSON(); };
    auto jsons   = patterns_ | rx::transform(getJSON) | rx::to_vector();

    auto master = *jsons.begin();
    jsons.erase(jsons.begin());

    JSON::removeDuplicates(master, jsons);
    jsons.insert(jsons.begin(), master);

    jsons = jsons | rx::filter([](auto &&j) { return !PatternSettings::removeMeta(j).empty(); })
            | rx::to_vector();

    return jsons;
}

PatternSettings &PatternMap::getSettingsByName(const QString &name)
{
    const std::string sName = name.toStdString();
    auto it                 = std::find_if(patterns_.begin(), patterns_.end(), [&sName](auto pSets) {
        return pSets.pattern == sName;
    });

    if (it == patterns_.end())
        throw std::runtime_error("Pattern '" + name.toStdString() + "' does not exist");

    return *it;
}

QStringList PatternMap::list()
{
    QStringList list;
    for (const auto &pattern : patterns_)
        list.push_back(QString::fromStdString(pattern.pattern));
    return list;
}

void PatternMap::remove(const QString &patternName)
{
    const std::string toFind = patternName.toStdString();
    auto it                  = std::find_if(patterns_.begin(), patterns_.end(), [&toFind](auto &&pSets) {
        return pSets.pattern == toFind;
    });

    if (it != patterns_.end())
        patterns_.erase(it);
}
} // namespace CAO
