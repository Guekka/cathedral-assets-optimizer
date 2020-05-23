/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "PatternMap.hpp"
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
    if (patterns_.empty())
        addPattern(PatternSettings{0, {"*"}});

    cleanPatterns();
}

void PatternMap::addPattern(PatternSettings pattern)
{
    patterns_.emplace(pattern.priority_, pattern);
}

PatternSettings PatternMap::getSettings(const QString &filePath) const
{
    assert(!patterns_.empty());

    auto matchWildcard = [&filePath](const std::string &wildcard) {
        using namespace wildcards;
        return isMatch(filePath.toStdString(), pattern{wildcard}, case_insensitive);
    };

    nlohmann::json merged;
    for (const auto &[_, patternSettings] : patterns_)
    {
        const auto &patterns = patternSettings.patterns_;
        if (std::any_of(patterns.cbegin(), patterns.cend(), matchWildcard))
            merged.merge_patch(patternSettings.getJSON());
    }

    return PatternSettings{merged};
}

const PatternSettings &PatternMap::getDefaultSettings() const
{
    return patterns_.begin()->second;
}

PatternSettings &PatternMap::getDefaultSettings()
{
    return patterns_.begin()->second;
}

void PatternMap::readFromUi(const MainWindow &window)
{
    patterns_ >>= pipes::for_each([&window](auto &&pair) { pair.second.readFromUi(window); });
}

void PatternMap::saveToUi(MainWindow &window) const
{
    patterns_ >>= pipes::for_each([&window](auto &&pair) { pair.second.saveToUi(window); });
}

nlohmann::json PatternMap::mergePattern(const nlohmann::json &main, const nlohmann::json &second) const
{
    auto patt1 = JSON::getValue<std::vector<std::string>>(main, PatternSettings::patternKey);
    auto patt2 = JSON::getValue<std::vector<std::string>>(second, PatternSettings::patternKey);
    patt1.insert(patt1.end(), patt2.begin(), patt2.end());
    patt1 = patt1 | rx::sort() | rx::uniq() | rx::to_vector();

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

void PatternMap::cleanPatterns()
{
    std::unordered_map<nlohmann::json, std::unordered_set<PatternSettings>> JsonToSets;

    for (auto [_, pattern] : patterns_)
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

        PatternSettings s{merged};
        patterns_.emplace(s.priority_, std::move(s));
    }
}

nlohmann::json PatternMap::getUnifiedJSON() const
{
    auto copy = *this;
    copy.cleanPatterns();
    auto patterns = std::move(copy.patterns_);

    auto getJSON = [](const auto &val) { return val.second.getJSON(); };
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
