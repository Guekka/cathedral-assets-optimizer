/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/Textures/TextureFormats.hpp"

#include <QObject>

constexpr double GigaByte = 1024 * 1024 * 1024;

namespace CAO {
enum OptimizationMode
{
    SingleMod   = 0,
    SeveralMods = 1
};
enum TextureResizingMode
{
    None,
    ByRatio,
    BySize
};

//Leaving place for intermediate levels
enum class GuiMode
{
    Invalid       = 0,
    QuickAutoPort = 1,
    Medium        = 5,
    Advanced      = 10
};

enum class GuiTheme
{
    Dark,
    Light
};

enum Games
{
    Morrowind,
    Oblivion,
    SkyrimLE,
    SkyrimSE,
    Fallout3,
    FalloutNewVegas,
    Fallout4
};

} // namespace CAO

Q_DECLARE_METATYPE(CAO::OptimizationMode)
Q_DECLARE_METATYPE(CAO::TextureResizingMode)
Q_DECLARE_METATYPE(CAO::GuiMode)
Q_DECLARE_METATYPE(CAO::GuiTheme)
Q_DECLARE_METATYPE(DXGI_FORMAT)
