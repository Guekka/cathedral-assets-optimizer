/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "PatternMap.hpp"

namespace CAO {
void PatternMap::listPatterns(nlohmann::json json)
{
    patterns_.clear();
    //Retrieving all the patterns and converting them to regexes
    for (auto it = json.begin(); it != json.end(); ++it) {
        const int priority = (*it).value<int>("Priority", 0);
        QString regexString;
        if (it->contains("Pattern")) {
            const QString &patternString = QString::fromStdString(
                it->at("Pattern").get<std::string>());
            regexString = QRegularExpression::wildcardToRegularExpression(patternString);
        } else if (it->contains("Regex"))
            regexString = QString::fromStdString(it->at("Regex").get<std::string>());
        else
            continue; //Current JSON is not valid

        //We only know that the first profile will contain all the settings. The others might be incomplete
        if (!patterns_.empty()) {
            const PatternSettings &defaultSets = patterns_.begin()->second.second;
            const auto &defaultJson = defaultSets.getJSON().getInternalJSON();
            auto patchedJson = defaultJson;
            patchedJson.merge_patch(*it);
            *it = patchedJson;
        }

        QRegularExpression regex(regexString);
        regex.optimize();
        const std::pair<int, Pattern> pair(priority, {regex, PatternSettings(*it)});
        patterns_.insert(pair);

        if (patterns_.empty())
            throw std::runtime_error("There should always be at least one file pattern");
    }
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

void PatternMap::saveToUi(MainWindow &window)
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
                   [](const auto &pair) { return pair.second.second.getJSON().getInternalJSON(); });

    return nlohmann::json{vector};
}
} // namespace CAO
