/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Games.h"

Games::Games() {};

Games *Games::INSTANCE = nullptr;

void Games::setGame(const GameMode &newGame)
{
    game = newGame;

    switch (game)
    {
    case SSE:
        bsaFormat = baSSE;
        maxBsaSize = 2 * static_cast<double>(GigaByte);
        hasBsaTextures = true;
        //Using 2.37GB for max textures size since this bsa will always be compressed.
        //After some experiments, it is the maximum size to ensure the bsa will always
        //stay under the maximum size : ~2.1gb
        //TODO let the user choose the max size in the INI
        maxBsaTexturesSize = 2.37 * static_cast<double>(GigaByte);
        bsaExtension = ".bsa";
        meshesFileVersion = V20_2_0_7;
        meshesStream = 100;
        meshesUser = 12;
        animationFormat = HKPF_AMD64;
        texturesFormat = "BC7_UNORM";
        texturesIncompatibleExtensions << ".tga";
        texturesIncompatibleFormats << "B5G5R5A1_UNORM" << "B5G6R5_UNORM";
        iniPath = "settings/SkyrimSE/config.ini";
        logPath = "SkyrimSE_log.html";
        break;
    case TES5:
        bsaFormat = baFO3;
        maxBsaSize = 2 * static_cast<double>(GigaByte);
        hasBsaTextures = false;
        maxBsaTexturesSize = 0;
        bsaExtension = ".bsa";
        meshesFileVersion = V20_2_0_7;
        meshesStream = 83;
        meshesUser = 12;
        animationFormat = HKPF_WIN32;
        texturesFormat = "BC3_UNORM";
        //texturesIncompatibleExtensions; //None
        texturesIncompatibleFormats << "BC7_UNORM";
        iniPath = "settings/TES5/config.ini";
        logPath = "TES5_log.html";
        break;
    }
}

GameMode Games::getGame() const
{
    return game;
}

Games* Games::getInstance()
{
    if(!Games::INSTANCE)
        Games::INSTANCE = new Games();

    return Games::INSTANCE;
}

bsa_archive_type_e Games::GetBsaFormat() const
{
    return bsaFormat;
}

double Games::getBsaMaxSize() const
{
    return maxBsaSize;
}

double Games::getBsaTexturesMaxSize() const
{
    return maxBsaTexturesSize;
}

QString Games::getBsaExtension() const
{
    return bsaExtension;
}

bool Games::getHasBsaTextures() const
{
    return hasBsaTextures;
}

uint Games::getMeshesUser() const
{
    return meshesUser;
}

uint Games::getMeshesStream() const
{
    return meshesStream;
}

NiFileVersion Games::getMeshesFileVersion() const
{
    return meshesFileVersion;
}

QString Games::getTexturesFormat() const
{
    return texturesFormat;
}

hkPackFormat Games::getAnimationsFormat() const
{
    return animationFormat;
}

QStringList Games::getTexturesIncompatibleExtensions() const
{
    return texturesIncompatibleExtensions;
}

QStringList Games::getTexturesIncompatibleFormats() const
{
    return texturesIncompatibleFormats;
}

QString Games::getIniPath() const
{
    return iniPath;
}

QString Games::getLogPath() const
{
    return logPath;
}
