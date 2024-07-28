/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "GeneralBSAModule.hpp"

#include "ui_GeneralBSAModule.h"
#include "utils/utils.hpp"

#include <QButtonGroup>

namespace cao {
GeneralBSAModule::GeneralBSAModule(QWidget *parent)
    : IWindowModule(parent)
    , ui_(std::make_unique<Ui::GeneralBSAModule>())
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

GeneralBSAModule::~GeneralBSAModule() = default;

void GeneralBSAModule::settings_to_ui(const Settings &settings)
{
    ui_->baseGroupBox->setChecked(true);
    const auto &profile = settings.current_profile();
    switch (profile.bsa_operation)
    {
        case BsaOperation::None: ui_->baseGroupBox->setChecked(false); break;
        case BsaOperation::Create: ui_->BSACreate->setChecked(true); break;
        case BsaOperation::Extract: ui_->BSAExtract->setChecked(true); break;
    }

    ui_->dontMakeLoaded->setChecked(!profile.bsa_make_dummy_plugins);
    ui_->dontRemoveFiles->setChecked(!profile.bsa_remove_files);
    ui_->dontCompress->setChecked(!profile.bsa_allow_compression);

    // Only show override toggle when it makes sense.
    if (profile.target_game != btu::Game::FNV)
    {
        ui_->makeOverrides->setChecked(false);
        ui_->makeOverrides->hide();
    }
    else
        ui_->makeOverrides->setChecked(profile.bsa_make_overrides);
}

void GeneralBSAModule::ui_to_settings(Settings &settings) const
{
    const bool base = ui_->baseGroupBox->isChecked();

    auto &profile = settings.current_profile();
    if (base && ui_->BSAExtract->isChecked())
        profile.bsa_operation = BsaOperation::Extract;
    else if (base && ui_->BSACreate->isChecked())
        profile.bsa_operation = BsaOperation::Create;
    else
        profile.bsa_operation = BsaOperation::None;

    profile.bsa_make_dummy_plugins = !ui_->dontMakeLoaded->isChecked();
    profile.bsa_remove_files       = !ui_->dontRemoveFiles->isChecked();
    profile.bsa_allow_compression  = !ui_->dontCompress->isChecked();
    profile.bsa_make_overrides     = ui_->makeOverrides->isChecked();
}

auto GeneralBSAModule::is_supported_game(btu::Game game) const noexcept -> bool
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

auto GeneralBSAModule::name() const noexcept -> QString
{
    return QObject::tr("BSA (General)");
}

} // namespace cao
