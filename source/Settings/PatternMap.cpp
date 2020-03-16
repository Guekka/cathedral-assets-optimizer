/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "PatternMap.hpp"
#include <string>

namespace CAO {
void PatternMap::listPatterns(nlohmann::json json)
{
    patterns_.clear();
    //Retrieving all the patterns and converting them to regexes
    for (auto &value : json)
    {
        if (!value.is_object())
            continue;

        //We only know that the first profile will contain all the settings. The others might be incomplete
        //NOTE We might want to use a struct instead of merging the first JSON into the others
        //Something like
        //struct Sets
        //    PatternSettings *defaultSets;
        //    PatternSettings *sets;

        if (!patterns_.empty())
        {
            const auto &defaultJson = getDefaultSettings().getJSON();
            auto patchedJson        = defaultJson;
            patchedJson.merge_patch(value);
            value = patchedJson;
        }

        addPattern(PatternSettings(value));
    }
    if (patterns_.empty())
        addPattern(PatternSettings{});

    cleanPatterns();
}

void PatternMap::addPattern(const PatternSettings &pattern)
{
    patterns_.emplace(pattern.priority_, pattern);
}

const PatternSettings &PatternMap::getSettings(const QString &filePath) const
{
    assert(!patterns_.empty());

    auto hasMatch = [&filePath](const auto &regex) { return regex.match(filePath).hasMatch(); };

    for (auto it = patterns_.crbegin(); it != patterns_.crend(); ++it)
    {
        const auto &regexes = it->second.regexes_;
        for (const auto &regex : regexes)
            if (hasMatch(regex))
                return it->second;
    }
    return getDefaultSettings();
}

PatternSettings &PatternMap::getSettings(const QString &filePath)
{
    return const_cast<PatternSettings &>(static_cast<const PatternMap *>(this)->getSettings(filePath));
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
    for (auto &pattern : patterns_)
        pattern.second.readFromUi(window);
}

void PatternMap::saveToUi(MainWindow &window) const
{
    for (auto &pattern : patterns_)
        pattern.second.saveToUi(window);
}

nlohmann::json PatternMap::mergePattern(const nlohmann::json &json1, const nlohmann::json &json2) const
{
    auto pKey1       = JSON::getValue<QStringList>(json1, PatternSettings::patternKey);
    auto pKey2       = JSON::getValue<QStringList>(json2, PatternSettings::patternKey);
    auto mergedPKeys = pKey1 + pKey2;

    auto rKey1       = JSON::getValue<QStringList>(json1, PatternSettings::regexKey);
    auto rKey2       = JSON::getValue<QStringList>(json2, PatternSettings::regexKey);
    auto mergedRKeys = rKey1 + rKey2;

    nlohmann::json merged = json1;
    JSON::setValue(merged, PatternSettings::patternKey, mergedPKeys);
    JSON::setValue(merged, PatternSettings::regexKey, mergedRKeys);

    return merged;
}

void PatternMap::cleanPatterns()
{
    //TODO If a JSON is a subset of another, it will not be merged.

    std::unordered_map<nlohmann::json, std::unordered_set<PatternSettings>> JsonToSets;

    for (auto [_, pattern] : patterns_)
    {
        auto cleanedJson = pattern.getJSONWithoutMeta();
        JsonToSets[cleanedJson].emplace(pattern);
    }

    patterns_.clear();

    for (auto &value : JsonToSets)
    {
        nlohmann::json merged;
        for (auto &settings : value.second)
        {
            auto json = settings.getJSON();
            merged    = mergePattern(merged, settings.getJSON());
        }
        PatternSettings s{merged};
        patterns_.emplace(s.priority_, std::move(s));
    }
}

nlohmann::json PatternMap::getUnifiedJSON() const
{
    std::vector<nlohmann::json> vector;
    vector.reserve(patterns_.size());

    std::transform(patterns_.cbegin(), patterns_.cend(), std::back_inserter(vector), [](const auto &pair) {
        return pair.second.getJSON();
    });

    return nlohmann::json{vector};
}

} // namespace CAO
