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

    bool hasDefault = any_of(patterns_, [](auto &&pSets) {
        return any_of(pSets.patterns_, [](const std::string &str) { return str == "*"; });
    });

    if (!hasDefault)
        addPattern(PatternSettings{0, {"*"}});

    cleanPatterns();
}

void PatternMap::addPattern(PatternSettings pattern)
{
    size_t idx = pattern.priority_;
    if (idx > patterns_.size())
    {
        pattern.priority_ = patterns_.size();
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
        if (any_of(patternSettings.patterns_, matchWildcard))
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

nlohmann::json PatternMap::mergePattern(const nlohmann::json &main, const nlohmann::json &second) const
{
    auto patt1 = JSON::getValue<std::vector<std::string>>(main, PatternSettings::patternKey);
    auto patt2 = JSON::getValue<std::vector<std::string>>(second, PatternSettings::patternKey);
    patt1.insert(patt1.end(), patt2.begin(), patt2.end());
    remove_duplicates(patt1);

    size_t prio1 = main.value<size_t>(PatternSettings::priorityKey, UINT_MAX);
    size_t prio2 = second.value<size_t>(PatternSettings::priorityKey, UINT_MAX);

    auto mergedPriority = std::min(prio1, prio2);
    if (mergedPriority == UINT_MAX)
        mergedPriority = 0;

    nlohmann::json merged = main;
    JSON::setValue(merged, PatternSettings::patternKey, patt1);
    JSON::setValue(merged, PatternSettings::priorityKey, mergedPriority);

    return merged;
}

void PatternMap::updatePatternsPriority()
{
    for (size_t i = 0; i < patterns_.size(); i++)
        patterns_[i].priority_ = i;
}

void PatternMap::cleanPatterns()
{
    std::unordered_map<nlohmann::json, std::unordered_set<PatternSettings>> JsonToSets;

    for (const auto &pattern : patterns_)
    {
        auto cleanedJson = PatternSettings::removeMeta(pattern.getJSON());
        auto curKeys     = JsonToSets | rx::transform([](auto &&p) { return p.first; }) | rx::to_vector();

        nlohmann::json key = [&cleanedJson, &curKeys] {
            for (const auto &cur : curKeys)
            {
                if (JSON::contains(cur, cleanedJson))
                    return cur;
            }
            return cleanedJson;
        }();

        JsonToSets[key].emplace(pattern);
    }

    patterns_.clear();

    for (auto &value : JsonToSets)
    {
        nlohmann::json merged = value.first;
        for (auto &settings : value.second)
            merged = mergePattern(merged, settings.getJSON());

        addPattern(PatternSettings{merged});
    }
}

nlohmann::json PatternMap::getUnifiedJSON() const
{
    auto copy = *this;
    copy.cleanPatterns();
    auto patterns = std::move(copy.patterns_);

    auto getJSON = [](const auto &val) { return val.getJSON(); };
    auto jsons   = patterns | rx::transform(getJSON) | rx::to_vector();

    auto master = *jsons.begin();
    jsons.erase(jsons.begin());

    JSON::removeDuplicates(master, jsons);
    jsons.insert(jsons.begin(), master);

    jsons = jsons | rx::filter([](auto &&j) { return !PatternSettings::removeMeta(j).empty(); })
            | rx::to_vector();

    return jsons;
}
} // namespace CAO
