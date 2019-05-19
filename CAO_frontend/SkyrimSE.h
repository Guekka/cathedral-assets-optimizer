/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"

namespace Ui {
    class MainWindow;
}

class SkyrimSE : public QMainWindow
{
    Q_DECLARE_TR_FUNCTIONS(MainWindow)

public:
    SkyrimSE();
    ~SkyrimSE() override;

private:
    QFileDialog *fileDialog{};
    Ui::MainWindow *ui;

    bool bDarkMode = true;
    bool bLockVariables = false;

    void saveUIToFile();
    void loadUIFromFile();
    void updateLog();
    void initProcess();
    void endProcess();

    int progressBarValue{};

    QSettings *settings;
};
