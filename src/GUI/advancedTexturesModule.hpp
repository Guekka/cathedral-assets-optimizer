/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "GUI/IWindowModule.hpp"

namespace Ui {
class AdvancedTexturesModule;
}

namespace CAO {
class ListDialog;

class AdvancedTexturesModule final : public IWindowModule
{
    Q_OBJECT

public:
    explicit AdvancedTexturesModule(QWidget *parent = nullptr);

    QString name() override;

private:
    std::unique_ptr<Ui::AdvancedTexturesModule> ui_;
    std::unique_ptr<ListDialog> textureFormatDialog_;

private:
    using ConnectionWrapper::connect;
    using ConnectionWrapper::connectWrapper;

    //IWindowModule
    void setUIData(const PatternSettings &pSets, const GeneralSettings &gSets) override;

    void connectAll(PatternSettings &patternSets, GeneralSettings &generalSets) override;

    bool isSupportedGame(Games game) override;
};
} // namespace CAO
