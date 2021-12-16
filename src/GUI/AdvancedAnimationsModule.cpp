/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "AdvancedAnimationsModule.hpp"
#include "Settings/Games.hpp"
#include "Settings/PatternSettings.hpp"
#include "Utils.hpp"
#include "ui_AdvancedAnimationsModule.h"

namespace CAO {
AdvancedAnimationsModule::AdvancedAnimationsModule(QWidget *parent)
    : IWindowModule(parent)
    , ui_(std::make_unique<Ui::AdvancedAnimationsModule>())
{
    ui_->setupUi(this);
}

AdvancedAnimationsModule::~AdvancedAnimationsModule() = default;

QString AdvancedAnimationsModule::name()
{
    return tr("Animations (Patterns)");
}

void AdvancedAnimationsModule::connectAll(PatternSettings &pSets, [[maybe_unused]] GeneralSettings &gSets)
{
    connectWrapper(*ui_->necessaryOpt, pSets.bAnimationsOptimization);
}

bool AdvancedAnimationsModule::isSupportedGame(Games game)
{
    return GameSettings::get(game).bAnimationsConvert();
}

} // namespace CAO
