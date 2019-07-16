/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#include "ui_mainWindow.h"
#include "Manager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_DECLARE_TR_FUNCTIONS(MainWindow)

public:
    MainWindow();
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    bool bLockVariables = false;

    void saveUi();
    void loadUi();
    void refreshUI();

    void resetUi();

    void setGameMode(const Games::GameMode &mode);

    void updateLog();
    void initProcess();
    void endProcess();
    void readProgress(const QString &text, const int &max, const int &value);

    void hideAdvancedSettings();
    void showAdvancedSettings();

    int progressBarValue{};

    QSettings *uiSettings;
    QSettings *settings;
    OptionsCAO options;

    std::unique_ptr<Manager> caoProcess;

    QTimer* timer;
};
