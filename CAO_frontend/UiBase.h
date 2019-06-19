/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#include "ui_UiBase.h"
#include "GameSelector.h"

namespace Ui {
class UiBase;
}

class UiBase : public QMainWindow
{
    Q_DECLARE_TR_FUNCTIONS(UiBase)

public:
    UiBase(const QString& logPath, const QString& iniPath);
    ~UiBase();

    void initProcess();
    void setupSettings();
    void updateLog();
    void closeEvent(QCloseEvent* event);
    void readProgress();

    virtual void loadUIFromFile() = 0;
    virtual void saveUIToFile() = 0;

protected:
    QFileDialog *fileDialog{};

    bool bDarkMode = true;
    bool bLockVariables = false;

    QTimer *timer;
    QSettings *settings;
    QProcess *caoProcess;

    QString m_logPath;
    QString m_iniPath;

private:
    Ui::UiBase *ui;
};

