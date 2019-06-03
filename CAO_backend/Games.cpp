/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Games.h"



void Games::setGame(const GameMode &newGame)
{
    switch (newGame)
    {
    case SSE:
        game = GameModeStructs::SSE();
        break;
    case TES5:
        game = GameModeStructs::TES5();
        break;
    }
}

bsa_archive_type_e Games::GetBsaFormat()
{
    return game.bsaFormat;
}

double Games::getBsaMaxSize()
{
    return game.maxBsaSize;
}

double Games::getBsaTexturesMaxSize()
{
    return game.maxBsaTexturesSize;
}

QString Games::getBsaExtension()
{
    return game.bsaExtension;
}

bool Games::hasBsaTextures()
{
    return game.hasBsaTextures;
}

uint Games::getMeshesUser()
{
    return game.meshesUser;
}

uint Games::getMeshesStream()
{
    return game.meshesStream;
}

NiFileVersion Games::getMeshesFileVersion()
{
    return game.meshesFileVersion;
}

QString Games::getTexturesFormat()
{
    return game.texturesFormat;
}

hkPackFormat Games::getAnimationsFormat()
{
    return game.animationFormat;
}



