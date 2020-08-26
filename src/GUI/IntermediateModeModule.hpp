/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "IWindowModule.hpp"
#include "ui_IntermediateModeModule.h"

namespace CAO {
class IntermediateModeModule : public IWindowModule
{
    Q_OBJECT

public:
    explicit IntermediateModeModule(QWidget *parent = nullptr);

    void connectAll(PatternSettings &pSets, GeneralSettings &gSets) override;

    QString name() override;

private:
    std::unique_ptr<Ui::IntermediateModeModule> ui_;
};
} // namespace CAO
