/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "IWindowModule.hpp"

namespace Ui {
class QuickAutoPortWindow;
}

namespace CAO {
class QuickAutoPortWindow : public IWindowModule
{
    Q_OBJECT

public:
    explicit QuickAutoPortWindow(QWidget *parent);

    void connectAll(PatternSettings &, GeneralSettings &) override;
    void disconnectAll() override;

private:
    std::unique_ptr<Ui::QuickAutoPortWindow> ui_;
};
} // namespace CAO
