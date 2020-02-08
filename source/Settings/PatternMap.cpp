/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "PatternMap.hpp"
#include <string>
namespace CAO {

constexpr auto patternKey = "Pattern";
constexpr auto regexKey = "Regex";
constexpr auto priorityKey = "Priority";

std::optional<QRegularExpression> PatternMap::getPatternRegex(const nlohmann::json &json)
{
    QString regexString;
    if (json.contains(patternKey)) {
        const QString &patternString = QString::fromStdString(
            json.at(patternKey).get<std::string>());
        regexString = QRegularExpression::wildcardToRegularExpression(patternString);
    } else if (json.contains(regexKey))
        regexString = QString::fromStdString(json.at(regexKey).get<std::string>());
    else
        return std::nullopt;

    QRegularExpression regex(regexString);
    return std::optional(regex);
}

std::optional<int> PatternMap::getPatternPriority(const nlohmann::json &json)
{
    if (!json.contains(priorityKey) || !json[priorityKey].is_number())
        return std::nullopt;

    return json[priorityKey].get<int>();
}

void PatternMap::listPatterns(nlohmann::json json)
{
    patterns_.clear();
    //Retrieving all the patterns and converting them to regexes
    for (auto &value : json) {
        if (!value.is_object())
            continue;

        const auto optPriority = getPatternPriority(value);
        constexpr int defaultPriority = 0;
        const int priority = optPriority ? *optPriority : defaultPriority;

        const auto optRegex = getPatternRegex(value);
        const QRegularExpression defaultRegex{QRegularExpression::wildcardToRegularExpression("*")};
        const QRegularExpression regex = optRegex ? *optRegex : defaultRegex;

        //We only know that the first profile will contain all the settings. The others might be incomplete
        if (!patterns_.empty()) {
            const PatternSettings &defaultSets = patterns_.begin()->second.second;
            const auto &defaultJson = defaultSets.getJSON();
            auto patchedJson = defaultJson;
            patchedJson.merge_patch(value);
            value = patchedJson;
        }

        addPattern(regex, priority, value);
    }
    if (patterns_.empty())
        addPattern(KeyType::Pattern, "*", 0);
}

void PatternMap::addPattern(PatternMap::KeyType type,
                            QString name,
                            int priority,
                            nlohmann::json json)
{
    QRegularExpression regex(name);
    if (type != KeyType::Regex)
        regex.setPattern(QRegularExpression::wildcardToRegularExpression(name));

    addPattern(regex, priority, json);
}

void PatternMap::addPattern(const QRegularExpression &regex, int priority, nlohmann::json json)
{
    auto jRegex = getPatternRegex(json);
    if (!(jRegex && *jRegex == regex))
        json[regexKey] = regex.pattern().toStdString();

    auto jPriority = getPatternPriority(json);
    if (!(jPriority && *jPriority == priority))
        json[priorityKey] = priority;

    const std::pair<int, Pattern> pair(priority, Pattern{regex, json});
    patterns_.insert(pair);
}

const PatternSettings &PatternMap::getSettings(const QString &filePath) const
{
    if (patterns_.empty())
        throw std::runtime_error("No file patterns found for current profile");

    const PatternSettings *result = &patterns_.begin()->second.second;
    for (auto it = patterns_.crbegin(); it != patterns_.crend(); ++it) {
        const auto &regex = it->second.first;
        const auto &match = regex.match(filePath);
        if (match.hasMatch()) {
            result = &it->second.second;
            break;
        }
    }

    return *result;
}

PatternSettings &PatternMap::getSettings(const QString &filePath)
{
    return const_cast<PatternSettings &>(
        static_cast<const PatternMap *>(this)->getSettings(filePath));
}

void PatternMap::readFromUi(const MainWindow &window)
{
    for (auto &pattern : patterns_)
        pattern.second.second.readFromUi(window);
}

void PatternMap::saveToUi(MainWindow &window) const
{
    for (auto &pattern : patterns_)
        pattern.second.second.saveToUi(window);
}

nlohmann::json PatternMap::getUnifiedJSON() const
{
    std::vector<nlohmann::json> vector;
    vector.reserve(patterns_.size());

    std::transform(patterns_.cbegin(),
                   patterns_.cend(),
                   std::back_inserter(vector),
                   [](const auto &pair) { return pair.second.second.getJSON(); });

    return nlohmann::json{vector};
}

} // namespace CAO
