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

void AdvancedMeshesModule::init(PatternSettings &pSets, [[maybe_unused]] GeneralSettings &gSets)
{
    connectGroupBox(ui_->mainGroupBox,
                    ui_->necessaryOptimizationRadioButton,
                    ui_->mediumOptimizationRadioButton,
                    ui_->fullOptimizationRadioButton);

    ui_->mainGroupBox->setChecked(pSets.iMeshesOptimizationLevel() != 0);
}

void AdvancedMeshesModule::connectAll(PatternSettings &pSets, [[maybe_unused]] GeneralSettings &gSets)
{
    auto *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui_->necessaryOptimizationRadioButton, 1);
    buttonGroup->addButton(ui_->mediumOptimizationRadioButton, 2);
    buttonGroup->addButton(ui_->fullOptimizationRadioButton, 3);

    connect(ui_->mainGroupBox,
            &QGroupBox::toggled,
            &pSets.iMeshesOptimizationLevel,
            [&pSets, buttonGroup](bool state) {
                pSets.iMeshesOptimizationLevel = state ? buttonGroup->checkedId() : 0;
            });

    connect(buttonGroup,
            QOverload<QAbstractButton *>::of(&QButtonGroup::buttonPressed),
            this,
            [&pSets, buttonGroup](QAbstractButton *button) {
                pSets.iMeshesOptimizationLevel = buttonGroup->id(button);
            });

    if (pSets.iMeshesOptimizationLevel() != 0)
        buttonGroup->button(pSets.iMeshesOptimizationLevel())->setChecked(true);

    connectWrapper(*ui_->ignoreHeadpartsCheckBox, pSets.bMeshesIgnoreHeadparts);
}

bool AdvancedMeshesModule::isSupportedGame(Games game)
{
    //Only LE and SSE are supported
    switch (game)
    {
        case Games::Morrowind:
        case Games::Oblivion: return false;
        case Games::SkyrimLE:
        case Games::SkyrimSE: return true;
        case Games::Fallout3:
        case Games::FalloutNewVegas:
        case Games::Fallout4: return false;
    }
    return false;
}

QString AdvancedMeshesModule::name()
{
    return tr("Meshes (Patterns)");
}

} // namespace CAO
