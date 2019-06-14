/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#include "TES5.h"
#include "SSE.h"
#include "FO4.h"
#include "selectiondialog.h"

class GameSelector : public QObject
{
public:
    static GameSelector* getInstance();
    ~GameSelector();

    int mainProcess(bool ignoreIni);

    QString getChoiceFromSelectorWindow();
    QString getChoiceFromIni();
    int showGameWindow(const QString& choice);
    void saveGameToIni(const QString& game);

private:
    GameSelector();

    static GameSelector* INSTANCE;
    QMainWindow *window = nullptr;
};

