/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "QuickAutoPortWindow.hpp"
#include "Settings/GeneralSettings.hpp"
#include "Settings/PatternSettings.hpp"
#include "Utils.hpp"
#include "ui_QuickAutoPortWindow.h"

namespace CAO {
QuickAutoPortWindow::QuickAutoPortWindow(QWidget *parent)
    : IWindowModule(parent)
    , ui_(new Ui::QuickAutoPortWindow)
{
    ui_->setupUi(this);
}

void QuickAutoPortWindow::connectAll(PatternSettings &pSets, GeneralSettings &gSets)
{
    gSets.bBSAExtractFromBSA = true;

    gSets.bBSACreateDummies   = true;
    gSets.bBSACompressArchive = true;

    gSets.bBSACompact      = true;
    gSets.bBSADeleteBackup = false;

    pSets.bTexturesNecessary = true;
    pSets.bTexturesCompress  = false;
    pSets.bTexturesMipmaps   = false;
    pSets.bTexturesLandscapeAlpha = true;

    pSets.eTexturesResizingMode = None;

    pSets.eTexturesFormat = pSets.eTexturesFormat.value_or(DXGI_FORMAT_BC7_UNORM);

    pSets.bTexturesForceConvert = false;

    pSets.iMeshesOptimizationLevel = 1;
}

} // namespace CAO
