/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "ProgressWindow.hpp"

namespace CAO {
ProgressWindow::ProgressWindow(QWidget *parent)
    : QWidget(parent)
    , ui(std::make_unique<Ui::ProgressWindow>())
{
    ui->setupUi(this);
}
} // namespace CAO
