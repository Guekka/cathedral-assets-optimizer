/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings.hpp"
#include "Utils/TypeConvert.hpp"

namespace CAO {
class PatternMap
{
public:
    void listPatterns(nlohmann::json json);
    void addPattern(const PatternSettings &pattern);

    const PatternSettings &getSettings(const QString &filePath) const;
    PatternSettings &getSettings(const QString &filePath);

    const PatternSettings &getDefaultSettings() const;
    PatternSettings &getDefaultSettings();

    void cleanPatterns();
    nlohmann::json getUnifiedJSON() const;

    void readFromUi(const MainWindow &window);
    void saveToUi(MainWindow &window) const;

    auto &get() { return patterns_; }
    const auto &get() const { return patterns_; }

private:
    std::map<size_t, PatternSettings> patterns_; //TODO several patterns with same priority

    //! \note It is assumed both patterns share the same keys
    nlohmann::json mergePattern(const nlohmann::json &json1, const nlohmann::json &json2) const;
};
} // namespace CAO
