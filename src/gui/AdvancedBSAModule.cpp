/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "AdvancedBSAModule.hpp"

#include "ui_AdvancedBSAModule.h"
#include "utils/utils.hpp"

#include <QButtonGroup>

namespace cao {
AdvancedBSAModule::AdvancedBSAModule(QWidget *parent)
    : IWindowModule(parent)
    , ui_(std::make_unique<Ui::AdvancedBSAModule>())
{
    ui_->setupUi(this);

    connect_group_box(ui_->baseGroupBox, ui_->BSACreate, ui_->BSAExtract);

    auto *button_group = new QButtonGroup(this); // NOLINT(cppcoreguidelines-owning-memory)
    button_group->addButton(ui_->BSACreate);
    button_group->addButton(ui_->BSAExtract);

    connect(ui_->baseGroupBox, &QGroupBox::toggled, this, [button_group, this](bool state) {
        // By default, check bsa create
        if (state)
            ui_->BSACreate->setChecked(true);
    });
}

AdvancedBSAModule::~AdvancedBSAModule() = default;

void AdvancedBSAModule::settings_to_ui(const Settings &settings)
{
    ui_->baseGroupBox->setChecked(true);
    switch (settings.current_profile().bsa_operation)
    {
        case BsaOperation::None: ui_->baseGroupBox->setChecked(false); break;
        case BsaOperation::Create: ui_->BSACreate->setChecked(true); break;
        case BsaOperation::Extract: ui_->BSAExtract->setChecked(true); break;
    }

    ui_->dontMakeLoaded->setChecked(!settings.current_profile().bsa_make_dummy_plugins);
}

void AdvancedBSAModule::ui_to_settings(Settings &settings) const
{
    const bool base = ui_->baseGroupBox->isChecked();

    if (base && ui_->BSAExtract->isChecked())
        settings.current_profile().bsa_operation = BsaOperation::Extract;
    else if (base && ui_->BSACreate->isChecked())
        settings.current_profile().bsa_operation = BsaOperation::Create;
    else
        settings.current_profile().bsa_operation = BsaOperation::None;

    settings.current_profile().bsa_make_dummy_plugins = !ui_->dontMakeLoaded->isChecked();
}

auto AdvancedBSAModule::is_supported_game(btu::Game game) const noexcept -> bool
{
    switch (game)
    {
        case btu::Game::TES3:
        case btu::Game::TES4:
        case btu::Game::SLE:
        case btu::Game::SSE:
        case btu::Game::FNV:
        case btu::Game::FO4:
        case btu::Game::Starfield: return true;
        case btu::Game::Custom: return false;
    }
    return false;
}

auto AdvancedBSAModule::name() const noexcept -> QString
{
    return QObject::tr("BSA (General)");
}

} // namespace cao
