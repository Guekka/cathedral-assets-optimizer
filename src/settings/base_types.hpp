/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QObject>

namespace cao {

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
    QuickOptimize,
    Medium,
    Advanced,
};

enum class GuiTheme
{
    Dark,
    Light
};

} // namespace cao

Q_DECLARE_METATYPE(cao::OptimizationMode)
Q_DECLARE_METATYPE(cao::TextureResizingMode)
Q_DECLARE_METATYPE(cao::GuiMode)
Q_DECLARE_METATYPE(cao::GuiTheme)
