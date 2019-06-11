/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#include "ui_TES5.h"
#include "GameSelector.h"

namespace Ui {
    class TES5;
}

class TES5 : public QMainWindow
{
    Q_DECLARE_TR_FUNCTIONS(MainWindow)

public:
    TES5();
    ~TES5();

private:
    QFileDialog *fileDialog{};
    Ui::TES5 *ui;

    bool bDarkMode = true;
    bool bLockVariables = false;

    void saveUIToFile();
    void loadUIFromFile();
    void updateLog();
    void initProcess();

    int progressBarValue{};

    QSettings *settings;

    QProcess *caoProcess;
};
