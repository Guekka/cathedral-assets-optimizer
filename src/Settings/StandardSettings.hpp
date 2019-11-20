/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "GUI/ui_mainWindow.h"
#include "pch.hpp"

namespace CAO {
using json = nlohmann::json;
namespace StandardSettings {
Q_NAMESPACE
enum OptimizationMode
{
    SingleMod = 0,
    SeveralMods = 1
};
Q_ENUM_NS(OptimizationMode)

enum StandardKey
{
    bDebugLog,
    bDryRun,
    eMode,
    sUserPath,

    bBsaExtract,
    bBsaCreate,
    bBsaDeleteBackup,
    bBsaProcessContent,

    bTexturesNecessary,
    bTexturesCompress,
    bTexturesMipmaps,

    bTexturesResizeSize,
    iTexturesTargetHeight,
    iTexturesTargetWidth,

    bTexturesResizeRatio,
    iTexturesTargetWidthRatio,
    iTexturesTargetHeightRatio,

    iMeshesOptimizationLevel,

    bMeshesHeadparts,
    bMeshesResave,

    bAnimationsOptimization
};
const json &toJSONKey(const StandardKey &key, const json &json);
json &toJSONKey(const StandardKey &key, json &json);

#ifdef GUI
void saveToUi(Ui::MainWindow *ui, const nlohmann::json &json);
void readFromUi(Ui::MainWindow *ui, json &json);
#endif

}; // namespace StandardSettings
} // namespace CAO
