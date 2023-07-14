/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "AdvancedMeshesModule.hpp"

#include "Utils.hpp"
#include "settings/per_file_settings.hpp"
#include "ui_AdvancedMeshesModule.h"

#include <QButtonGroup>

namespace cao {
AdvancedMeshesModule::AdvancedMeshesModule(QWidget *parent)
    : IWindowModule(parent)
    , ui_(std::make_unique<Ui::AdvancedMeshesModule>())
{
    ui_->setupUi(this);

    connectGroupBox(ui_->mainGroupBox,
                    ui_->necessaryOptimizationRadioButton,
                    ui_->fullOptimizationRadioButton);
}

AdvancedMeshesModule::~AdvancedMeshesModule() = default;

QString AdvancedMeshesModule::name() const noexcept
{
    return tr("Meshes (Patterns)");
}

void AdvancedMeshesModule::set_ui_data(const Settings &settings)
{
    /*
     ui_->mainGroupBox->setChecked(pSets.iMeshesOptimizationLevel() != 0);

auto *buttonGroup = new QButtonGroup(this);
buttonGroup->addButton(ui_->necessaryOptimizationRadioButton, 1);
buttonGroup->addButton(ui_->fullOptimizationRadioButton, 2);

connect(ui_->mainGroupBox,
   &QGroupBox::toggled,
   &pSets.iMeshesOptimizationLevel,
   [&pSets, buttonGroup](bool state) {
       //The default is the last, no idea why
       if (state)
           buttonGroup->button(1)->setChecked(true);

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

     TODO
     */
}

void AdvancedMeshesModule::ui_to_settings(Settings &settings) const {}

bool AdvancedMeshesModule::is_supported_game(btu::Game game) const noexcept
{
    return game == btu::Game::SSE || game == btu::Game::SLE; // TODO: check if this is correct
}

} // namespace cao
