/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "IWindowModule.hpp"
#include "Settings/GeneralSettings.hpp"

namespace CAO {

IWindowModule::IWindowModule(QWidget *parent)
    : QWidget(parent)
{
}

void IWindowModule::setup(PatternSettings &pSets, GeneralSettings &gSets)
{
    if (!isSupportedGame(gSets.eGame()))
    {
        this->setDisabled(true);
        return;
    }
    this->setEnabled(true);

    init(pSets, gSets);
    connectAll(pSets, gSets);
}

void IWindowModule::disconnectAll()
{
    for (auto *obj : this->findChildren<QObject *>())
        obj->disconnect();
}

void IWindowModule::init([[maybe_unused]] PatternSettings &pSets, [[maybe_unused]] GeneralSettings &gSets) {}

} // namespace CAO
