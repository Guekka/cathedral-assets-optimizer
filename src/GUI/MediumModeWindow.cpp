/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MediumModeWindow.hpp"

namespace CAO {
MediumModeWindow::MediumModeWindow(QWidget *parent)
    : IWindowModule(parent)
    , ui(std::make_unique<Ui::MediumModeWindow>())
{
    ui->setupUi(this);
}

void MediumModeWindow::connectAll(PatternSettings &, GeneralSettings &) {}

} // namespace CAO
