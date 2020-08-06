/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings/PatternSettings.hpp"
#include "Utils/TypeConvert.hpp"

namespace CAO {
class PatternMap
{
public:
    void listPatterns(const nlohmann::json &json);
    void addPattern(PatternSettings pattern);

    PatternSettings getSettings(const QString &filePath) const;

    const PatternSettings &getDefaultSettings() const;
    PatternSettings &getDefaultSettings();

    PatternSettings &getSettingsByName(const QString &name);

    QStringList list();
    void remove(const QString &patternName);

    nlohmann::json getUnifiedJSON() const;

    auto &get() { return patterns_; }
    const auto &get() const { return patterns_; }

private:
    std::vector<PatternSettings> patterns_;

    void updatePatternsPriority();
};
} // namespace CAO
