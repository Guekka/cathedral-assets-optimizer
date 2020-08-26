/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "IWindowModule.hpp"
#include "ui_AdvancedMeshesModule.h"

namespace Ui {
class AdvancedMeshesModule;
}

namespace CAO {
class AdvancedMeshesModule : public IWindowModule
{
public:
    explicit AdvancedMeshesModule(QWidget *parent = nullptr);

    void connectAll(PatternSettings &pSets, GeneralSettings &gSets) override;

    QString name() override;

private:
    Ui::AdvancedMeshesModule *ui_;
};
} // namespace CAO
