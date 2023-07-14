/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "AdvancedAnimationsModule.hpp"

#include "Utils.hpp"
#include "settings/per_file_settings.hpp"
#include "ui_AdvancedAnimationsModule.h"

namespace cao {
AdvancedAnimationsModule::AdvancedAnimationsModule(QWidget *parent)
    : IWindowModule(parent)
    , ui_(std::make_unique<Ui::AdvancedAnimationsModule>())
{
    ui_->setupUi(this);
}

AdvancedAnimationsModule::~AdvancedAnimationsModule() = default;

void AdvancedAnimationsModule::set_ui_data(const Settings &settings)
{
    /* TODO
     connectWrapper(*ui_->necessaryOpt, pSets.bAnimationsOptimization);
     */
}

void AdvancedAnimationsModule::ui_to_settings(Settings &settings) const
{
    // TODO
}

bool AdvancedAnimationsModule::is_supported_game(btu::Game game) const noexcept
{
    return game == btu::Game::SSE || game == btu::Game::SLE;
}

auto AdvancedAnimationsModule::name() const noexcept -> QString
{
    return tr("Animations (Patterns)");
}

} // namespace cao
