/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "AdvancedAnimationsModule.hpp"

#include "ui_AdvancedAnimationsModule.h"

namespace cao {
AdvancedAnimationsModule::AdvancedAnimationsModule(QWidget *parent)
    : IWindowModule(parent)
    , ui_(std::make_unique<Ui::AdvancedAnimationsModule>())
{
    ui_->setupUi(this);
}

AdvancedAnimationsModule::~AdvancedAnimationsModule() = default;

void AdvancedAnimationsModule::settings_to_ui(const Settings &settings)
{
    const auto &pfs = current_per_file_settings(settings);
    ui_->necessaryOpt->setChecked(pfs.hkx_optimize != OptimizeType::None);
}

void AdvancedAnimationsModule::ui_to_settings(Settings &settings) const
{
    // We have basically nothing to do here. The only thing we need to do is to set the hkx_target
    // If we are here, we already know that the game is supported

    auto &pfs = current_per_file_settings(settings);

    pfs.hkx_target   = settings.current_profile().target_game;
    pfs.hkx_optimize = ui_->necessaryOpt->isChecked() ? OptimizeType::Normal : OptimizeType::None;
}

auto AdvancedAnimationsModule::is_supported_game(btu::Game game) const noexcept -> bool
{
    return game == btu::Game::SSE || game == btu::Game::SLE;
}

auto AdvancedAnimationsModule::name() const noexcept -> QString
{
    return QObject::tr("Animations (Patterns)");
}

} // namespace cao
