/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"

enum GameMode { Oldrim, SSE } ;

class Games
{
private:
    static GameMode game;
public:
    static bsa_archive_type_e GetBsaFormat();
    static double getBsaMaxSize();
    static double getBsaTexturesMaxSize();
    static QString getBsaExtension();
    static NiFileVersion getMeshesFileVersion();
    static uint getMeshesStream();
    static uint getMeshesUser();
    static hkPackFormat getAnimationsFormat();
    static QString getTexturesFormat();

    static void setGame(const GameMode& newGame);
};

