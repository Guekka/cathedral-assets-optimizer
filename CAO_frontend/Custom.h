/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#include "ui_Custom.h"

namespace Ui {
class Custom;
}

class Custom : public QMainWindow
{
    Q_DECLARE_TR_FUNCTIONS(SSE)

public:
    Custom();
    ~Custom();

private:
    Ui::Custom *ui;

    bool bLockVariables = false;

    void saveUIToFile();
    void loadUIFromFile();
    void refreshUI();

    void getGameFromIni();
    void getGameFromUi();
    void setGameMode();

    void updateLog();
    void initProcess();
    void endProcess();
    void readProgress();

    void hideAdvancedSettings();
    void showAdvancedSettings();

    void closeEvent(QCloseEvent* event);
    QTimer *timer;

    int progressBarValue{};

    QSettings *settings;
    QSettings *commonSettings;

    QProcess *caoProcess;

    QString game;

    QString logPath;
    QString iniPath;
};
