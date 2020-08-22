/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "GUI/IWindowModule.hpp"

namespace Ui {
class AdvancedTexturesWindow;
}

namespace CAO {
class AdvancedTexturesWindow final : public IWindowModule
{
    Q_OBJECT

public:
    explicit AdvancedTexturesWindow(QWidget *parent = nullptr);
    void connectAll(PatternSettings &patternSets, GeneralSettings &generalSets) override;

    QString name() override;

private:
    std::unique_ptr<Ui::AdvancedTexturesWindow> ui_;
};
} // namespace CAO
