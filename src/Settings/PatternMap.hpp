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

    void cleanPatterns();
    nlohmann::json getUnifiedJSON() const;

    void readFromUi(const MainWindow &window);
    void saveToUi(MainWindow &window) const;

    auto &get() { return patterns_; }
    const auto &get() const { return patterns_; }

private:
    std::map<size_t, PatternSettings> patterns_;

    //! \note It is assumed both patterns share the same keys
    nlohmann::json mergePattern(const nlohmann::json &main, const nlohmann::json &second) const;
};
} // namespace CAO
