/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings.hpp"

#define REGISTER_SETTING(type, name, key) \
    QJSONValueWrapper<type> name{json_, nlohmann::json_pointer<nlohmann::json>{key}};

namespace CAO {
class CommonSettings : public Settings
{
public:
    CommonSettings(){};
    CommonSettings(const CommonSettings &other) { json_ = other.json_; }
    CommonSettings(CommonSettings &&other) { json_ = std::move(other.json_); }

    std::optional<QString> isValid() const override { return std::nullopt; }

    REGISTER_SETTING(QString, sProfile, "/Profile")
    REGISTER_SETTING(bool, bDarkMode, "/DarkMode")
    REGISTER_SETTING(bool, bShowTutorials, "/ShowTutorials")
    REGISTER_SETTING(bool, bFirstStart, "/FirstStart")
    REGISTER_SETTING(bool, bDebugLog, "/DebugLog");

    REGISTER_SETTING(int, iGPUIndex, "/GPUIndex")
};

} // namespace CAO

#undef REGISTER_SETTING
