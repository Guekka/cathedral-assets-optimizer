/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "QuickAutoPortWindow.hpp"
#include "Settings/GeneralSettings.hpp"
#include "Settings/PatternSettings.hpp"
#include "Utils.hpp"
#include "ui_QuickAutoPortWindow.h"

namespace CAO {
QuickAutoPortWindow::QuickAutoPortWindow(QWidget *parent)
    : IWindowModule(parent)
    , ui_(new Ui::QuickAutoPortWindow)
{
    ui_->setupUi(this);
}

void QuickAutoPortWindow::connectAll(PatternSettings &, GeneralSettings &) {}

} // namespace CAO
