/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings.hpp"

namespace CAO {
class PatternMap
{
public:
    using Pattern = std::pair<QRegularExpression, PatternSettings>;
    enum class KeyType { Pattern, Regex };

    std::optional<QRegularExpression> getPatternRegex(const nlohmann::json &json);
    std::optional<int> getPatternPriority(const nlohmann::json &json);

    void listPatterns(nlohmann::json json);
    void addPattern(KeyType type, QString name, int priority, nlohmann::json json = {});
    void addPattern(const QRegularExpression &regex, int priority, nlohmann::json json = {});

    const PatternSettings &getSettings(const QString &filePath) const;
    PatternSettings &getSettings(const QString &filePath);

    nlohmann::json getUnifiedJSON() const;

    void readFromUi(const MainWindow &window);
    void saveToUi(MainWindow &window) const;

    auto &get() { return patterns_; }
    const auto &get() const { return patterns_; }

private:
    std::multimap<int, Pattern> patterns_;
};
} // namespace CAO
