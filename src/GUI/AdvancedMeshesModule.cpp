/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "AdvancedMeshesModule.hpp"
#include "Settings/GeneralSettings.hpp"
#include "Settings/PatternSettings.hpp"
#include "Utils.hpp"
#include "ui_AdvancedMeshesModule.h"

namespace CAO {
AdvancedMeshesModule::AdvancedMeshesModule(QWidget *parent)
    : IWindowModule(parent)
    , ui_(std::make_unique<Ui::AdvancedMeshesModule>())
{
    ui_->setupUi(this);
}

AdvancedMeshesModule::~AdvancedMeshesModule() = default;

void AdvancedMeshesModule::connectAll(PatternSettings &pSets, GeneralSettings &gSets)
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

QString AdvancedMeshesModule::name()
{
    return tr("Meshes (Patterns)");
}
} // namespace CAO
