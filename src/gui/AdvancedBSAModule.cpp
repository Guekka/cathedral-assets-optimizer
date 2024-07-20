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
}

AdvancedBSAModule::~AdvancedBSAModule() = default;

void AdvancedBSAModule::settings_to_ui(const Settings &settings)
{   
    auto &pfs = current_per_file_settings(settings);
    ui_->packFile->setChecked(pfs.pack);
}

void AdvancedBSAModule::ui_to_settings(Settings &settings) const
{
    auto &pfs = current_per_file_settings(settings);
    pfs.pack  = ui_->packFile->isChecked();
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
    return QObject::tr("BSA (Patterns)");
}

} // namespace cao
