/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings/Profiles.hpp"

void initSettings()
{
    const QString &profileName = "TestingProfile";
    CAO::Profiles::getInstance().create(profileName);
    auto &profile = CAO::Profiles::getInstance().setCurrent(profileName);

    //Resetting
    profile.getGeneralSettings() = {};
    profile.getPatterns().get()  = {{0, CAO::PatternSettings{}}};
}
