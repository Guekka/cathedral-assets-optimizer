/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Settings.hpp"
#include "Settings/Profiles.hpp"

namespace CAO {

Settings::Settings(const nlohmann::json &j)
    : json_(nlohmann::json(j))
{
}

Settings::Settings(const Settings &other)
    : json_(other.json_)
{
}

Settings::Settings(Settings &&other)
    : json_(std::move(other.json_))
{
}

void Settings::operator=(const Settings &other)
{
    json_ = other.json_;
}

void Settings::saveToUi(MainWindow &window) const
{
    for (const auto &set : uiSyncList_)
        set.save(window, json_);
}

void Settings::readFromUi(const MainWindow &window)
{
    for (const auto &set : uiSyncList_)
        set.read(window, json_);
}

} // namespace CAO
