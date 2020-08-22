/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MeshesAdvancedWindow.hpp"
#include "Settings/GeneralSettings.hpp"
#include "Settings/PatternSettings.hpp"
#include "Utils.hpp"

namespace CAO {
MeshesAdvancedWindow::MeshesAdvancedWindow(QWidget *parent)
    : IWindowModule(parent)
    , ui_(new Ui::MeshesAdvancedWindow)
{
    ui_->setupUi(this);
}

void MeshesAdvancedWindow::connectAll(PatternSettings &pSets, GeneralSettings &gSets)
{
    //Only LE and SSE are supported
    switch (gSets.eGame())
    {
        case SkyrimLE:
        case SkyrimSE: setEnabled(true); break;
        default: setEnabled(false); return;
    }

    connectGroupBox(ui_->mainGroupBox,
                    ui_->necessaryOptimizationRadioButton,
                    ui_->mediumOptimizationRadioButton,
                    ui_->fullOptimizationRadioButton);

    ui_->mainGroupBox->setChecked(pSets.iMeshesOptimizationLevel() == 0);

    connect(ui_->mainGroupBox, &QGroupBox::toggled, &pSets.iMeshesOptimizationLevel, [&pSets](bool state) {
        if (state)
            pSets.iMeshesOptimizationLevel = 0;
    });

    ui_->necessaryOptimizationRadioButton->setChecked(pSets.iMeshesOptimizationLevel() == 1);

    connect(ui_->necessaryOptimizationRadioButton,
            &QRadioButton::toggled,
            &pSets.iMeshesOptimizationLevel,
            [&pSets](bool state) {
                if (state)
                    pSets.iMeshesOptimizationLevel = 1;
            });

    ui_->mediumOptimizationRadioButton->setChecked(pSets.iMeshesOptimizationLevel() == 2);

    connect(ui_->mediumOptimizationRadioButton,
            &QRadioButton::toggled,
            &pSets.iMeshesOptimizationLevel,
            [&pSets](bool state) {
                if (state)
                    pSets.iMeshesOptimizationLevel = 2;
            });

    ui_->fullOptimizationRadioButton->setChecked(pSets.iMeshesOptimizationLevel() == 2);

    connect(ui_->fullOptimizationRadioButton,
            &QRadioButton::toggled,
            &pSets.iMeshesOptimizationLevel,
            [&pSets](bool state) {
                if (state)
                    pSets.iMeshesOptimizationLevel = 3;
            });

    connectWrapper(*ui_->ignoreHeadpartsCheckBox, pSets.bMeshesIgnoreHeadparts);
}

QString MeshesAdvancedWindow::name()
{
    return tr("Meshes (Patterns)");
}
} // namespace CAO
