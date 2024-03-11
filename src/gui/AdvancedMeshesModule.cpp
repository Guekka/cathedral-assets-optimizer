/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "AdvancedMeshesModule.hpp"

#include "ui_AdvancedMeshesModule.h"
#include "utils/utils.hpp"

#include <QButtonGroup>

namespace cao {
AdvancedMeshesModule::AdvancedMeshesModule(QWidget *parent)
    : IWindowModule(parent)
    , ui_(std::make_unique<Ui::AdvancedMeshesModule>())
{
    ui_->setupUi(this);

    connect_group_box(ui_->mainGroupBox,
                      ui_->necessaryOptimizationRadioButton,
                      ui_->fullOptimizationRadioButton);

    auto *button_group = new QButtonGroup(this); // NOLINT(cppcoreguidelines-owning-memory)
    button_group->addButton(ui_->necessaryOptimizationRadioButton, 1);
    button_group->addButton(ui_->fullOptimizationRadioButton, 2);

    connect(ui_->mainGroupBox, &QGroupBox::toggled, this, [button_group](bool state) {
        // By default, the button group will have the last button checked. We don't want that.
        if (state)
            button_group->button(1)->setChecked(true);
    });
}

AdvancedMeshesModule::~AdvancedMeshesModule() = default;

auto AdvancedMeshesModule::name() const noexcept -> QString
{
    return tr("Meshes (Patterns)");
}

void AdvancedMeshesModule::settings_to_ui(const Settings &settings)
{
    ui_->mainGroupBox->setChecked(true);
    switch (current_per_file_settings(settings).nif_optimize)
    {
        case OptimizeType::None: ui_->mainGroupBox->setChecked(false); break;
        case OptimizeType::Normal: ui_->necessaryOptimizationRadioButton->setChecked(true); break;
        case OptimizeType::Forced: ui_->fullOptimizationRadioButton->setChecked(true); break;
    }
}

void AdvancedMeshesModule::ui_to_settings(Settings &settings) const
{
    if (ui_->necessaryOptimizationRadioButton->isChecked())
        current_per_file_settings(settings).nif_optimize = OptimizeType::Normal;
    else if (ui_->fullOptimizationRadioButton->isChecked())
        current_per_file_settings(settings).nif_optimize = OptimizeType::Forced;
    else
        current_per_file_settings(settings).nif_optimize = OptimizeType::None;
}

auto AdvancedMeshesModule::is_supported_game(btu::Game game) const noexcept -> bool
{
    return game == btu::Game::SSE || game == btu::Game::SLE; // TODO: check if this is correct
}

} // namespace cao
