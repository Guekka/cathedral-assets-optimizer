/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "GameSelector.h"

GameSelector* GameSelector::INSTANCE = nullptr;

GameSelector::GameSelector() {}

GameSelector::~GameSelector()
{
    if(INSTANCE)
        delete INSTANCE;

    if(window)
        delete window;
}

GameSelector* GameSelector::getInstance()
{
    if(!INSTANCE)
        INSTANCE = new GameSelector();

    return INSTANCE;
}

int GameSelector::mainProcess(bool ignoreIni)
{
    if(window)
    {
        window->close();
        window = nullptr;
    }

    QString game;
    if(!ignoreIni)
        game = getChoiceFromIni();
    if(game.isEmpty())
        game = getChoiceFromSelectorWindow();

    return showGameWindow(game);
}

QString GameSelector::getChoiceFromIni()
{
    QSettings settings("settings/common/config.ini", QSettings::IniFormat, this);
    QString choice = settings.value("Game").toString();
    return choice;
}

QString GameSelector::getChoiceFromSelectorWindow()
{

    SelectionDialog dialog(QObject::tr("Game mode"), nullptr);
    dialog.disableCancel();
    dialog.addChoice("SSE", QObject::tr("Skyrim SE"), QVariant());
    dialog.addChoice("TES5", QObject::tr("Skyrim"), QVariant());
    dialog.exec();

    return dialog.getChoiceString();
}

int GameSelector::showGameWindow(const QString &choice)
{
    if(choice == "SSE")
        window = new SSE();
    else if(choice == "TES5")
        window = new TES5();
    else
        return 1;

    saveGameToIni(choice);
    window->show();

    return 0;
}

void GameSelector::saveGameToIni(const QString &game)
{
    QSettings settings("settings/common/config.ini", QSettings::IniFormat, this);
    settings.setValue("Game", game);
}
