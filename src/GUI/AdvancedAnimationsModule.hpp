/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "IWindowModule.hpp"

namespace Ui {
class AdvancedAnimationsModule;
}

namespace CAO {
class AdvancedAnimationsModule : public IWindowModule
{
public:
    explicit AdvancedAnimationsModule(QWidget *parent = nullptr);
    ~AdvancedAnimationsModule();

    QString name() override;

private:
    std::unique_ptr<Ui::AdvancedAnimationsModule> ui_;

private:
    using ConnectionWrapper::connect;
    using ConnectionWrapper::connectWrapper;

    void connectAll(PatternSettings &pSets, GeneralSettings &gSets) override;
    bool isSupportedGame(Games game) override;
};
} // namespace CAO
