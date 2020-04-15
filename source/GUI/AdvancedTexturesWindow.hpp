/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QWidget>

namespace Ui {
class AdvancedTexturesWindow;
}

class AdvancedTexturesWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AdvancedTexturesWindow(QWidget *parent = nullptr);
    ~AdvancedTexturesWindow();

private:
    Ui::AdvancedTexturesWindow *ui;
};

