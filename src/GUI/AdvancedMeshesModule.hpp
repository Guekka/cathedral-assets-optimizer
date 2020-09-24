/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "IWindowModule.hpp"

namespace Ui {
class AdvancedMeshesModule;
}

namespace CAO {
class AdvancedMeshesModule : public IWindowModule
{
    Q_OBJECT
public:
    explicit AdvancedMeshesModule(QWidget *parent = nullptr);
    ~AdvancedMeshesModule();

    QString name() override;

private:
    std::unique_ptr<Ui::AdvancedMeshesModule> ui_;

private:
    using ConnectionWrapper::connect;
    using ConnectionWrapper::connectWrapper;

    void setUIData(const PatternSettings &pSets, const GeneralSettings &gSets) override;

    void connectAll(PatternSettings &pSets, GeneralSettings &gSets) override;
    bool isSupportedGame(Games game) override;
};
} // namespace CAO
