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

    void listPatterns(nlohmann::json json);
    const PatternSettings &getSettings(const QString &filePath) const;
    PatternSettings &getSettings(const QString &filePath);

    nlohmann::json getUnifiedJSON() const;

    void readFromUi(const MainWindow &window);
    void saveToUi(MainWindow &window);

    std::multimap<int, Pattern> &get() { return patterns_; }
    const std::multimap<int, Pattern> &get() const { return patterns_; }

private:
    std::multimap<int, Pattern> patterns_;
};
} // namespace CAO
