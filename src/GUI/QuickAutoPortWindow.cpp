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

    ui_->extractBSAsCheckbox->setChecked(!gSets.bBSACreate());
    ui_->createBSAsCheckbox->setChecked(gSets.bBSACreate());

    connect(ui_->createBSAsCheckbox, &QCheckBox::toggled, this, [this, &gSets](bool checked) {
        if (!checked)
            return;

        gSets.bBSACreate = true;
        ui_->extractBSAsCheckbox->setChecked(false);
        ui_->createBSAsCheckbox->setChecked(true);
    });
    connect(ui_->extractBSAsCheckbox, &QCheckBox::toggled, this, [this, &gSets](bool checked) {
        if (!checked)
            return;

        gSets.bBSACreate = false;
        ui_->extractBSAsCheckbox->setChecked(true);
        ui_->createBSAsCheckbox->setChecked(false);
    });

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

void QuickAutoPortWindow::disconnectAll() {}
} // namespace CAO
