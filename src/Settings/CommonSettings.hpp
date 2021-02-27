/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "BaseTypes.hpp"
#include "Settings.hpp"
#include "Utils/QJSONValueWrapper.hpp"

#define REGISTER_SETTING(type, name, key) \
    QJSONValueWrapper<type> name{json_, nlohmann::json_pointer<nlohmann::json>{key}};

namespace CAO {
class CommonSettings : public Settings
{
public:
    CommonSettings(){};
    CommonSettings(const CommonSettings &other) { json_ = other.json_; }
    CommonSettings(CommonSettings &&other) noexcept { json_ = std::move(other.json_); }

    CommonSettings &operator=(const CommonSettings &other)
    {
        json_ = other.json_;
        return *this;
    }
    CommonSettings &operator=(CommonSettings &&other) noexcept
    {
        json_ = std::move(other.json_);
        return *this;
    }

    std::optional<QString> isValid() const override { return std::nullopt; }

    REGISTER_SETTING(QString, sProfile, "/Profile")
    REGISTER_SETTING(GuiTheme, eTheme, "/Theme")

    REGISTER_SETTING(bool, bRememberGUIMode, "/RememberGUIMode")
    REGISTER_SETTING(GuiMode, eGUIMode, "/GUIMode")

    REGISTER_SETTING(bool, bShowTutorials, "/ShowTutorials")
    REGISTER_SETTING(bool, bFirstStart, "/FirstStart")

    REGISTER_SETTING(int, iGPUIndex, "/GPUIndex")

    REGISTER_SETTING(bool, bDeleteEmptyFolders, "/DeleteEmptyFolders")
};

} // namespace CAO

#undef REGISTER_SETTING
