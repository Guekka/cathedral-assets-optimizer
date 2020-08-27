/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "IWindowModule.hpp"

namespace Ui {
class AdvancedBSAModule;
}

namespace CAO {
class AdvancedBSAModule : public IWindowModule
{
public:
    explicit AdvancedBSAModule(QWidget *parent = nullptr);
    ~AdvancedBSAModule();

private:
    std::unique_ptr<Ui::AdvancedBSAModule> ui_;

    void init(PatternSettings &pSets, GeneralSettings &gSets) override;
    void connectAll(PatternSettings &pSets, GeneralSettings &gSets) override;

    bool isSupportedGame(Games game) override;
};
} // namespace CAO
