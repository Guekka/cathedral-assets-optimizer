/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <nlohmann/json.hpp>

#include <QObject>

namespace cao {

enum class OptimizationMode : std::uint8_t
{
    SingleMod   = 0,
    SeveralMods = 1
};
enum class TextureResizingMode : std::uint8_t
{
    None,
    ByRatio,
    BySize
};

enum class GuiMode : std::uint8_t
{
    QuickOptimize,
    Medium,
    Advanced,
};

enum class GuiTheme : std::uint8_t
{
    Dark,
    Light
};

NLOHMANN_JSON_SERIALIZE_ENUM(cao::OptimizationMode,
                             {{cao::OptimizationMode::SingleMod, "SingleMod"},
                              {cao::OptimizationMode::SeveralMods, "SeveralMods"}})

NLOHMANN_JSON_SERIALIZE_ENUM(cao::TextureResizingMode,
                             {{cao::TextureResizingMode::None, "None"},
                              {cao::TextureResizingMode::ByRatio, "ByRatio"},
                              {cao::TextureResizingMode::BySize, "BySize"}})

NLOHMANN_JSON_SERIALIZE_ENUM(cao::GuiMode,
                             {{cao::GuiMode::QuickOptimize, "QuickOptimize"},
                              {cao::GuiMode::Medium, "Medium"},
                              {cao::GuiMode::Advanced, "Advanced"}})

NLOHMANN_JSON_SERIALIZE_ENUM(cao::GuiTheme, {{cao::GuiTheme::Dark, "Dark"}, {cao::GuiTheme::Light, "Light"}})

} // namespace cao

Q_DECLARE_METATYPE(cao::OptimizationMode)
Q_DECLARE_METATYPE(cao::TextureResizingMode)
Q_DECLARE_METATYPE(cao::GuiMode)
Q_DECLARE_METATYPE(cao::GuiTheme)
