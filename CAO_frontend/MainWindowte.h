/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindowte;
}

class MainWindowte : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindowte(QWidget *parent = nullptr);
    ~MainWindowte();

private:
    Ui::MainWindowte *ui;
};

