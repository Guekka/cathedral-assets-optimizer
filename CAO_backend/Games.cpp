/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Games.h"

Games::Games() {};

Games *Games::INSTANCE = nullptr;

void Games::setGame(const GameMode &newGame)
{
    switch (newGame)
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
        break;
    }
}

Games* Games::getInstance()
{
    if(!Games::INSTANCE)
        Games::INSTANCE = new Games();

    return Games::INSTANCE;
}

bsa_archive_type_e Games::GetBsaFormat()
{
    return bsaFormat;
}

double Games::getBsaMaxSize()
{
    return maxBsaSize;
}

double Games::getBsaTexturesMaxSize()
{
    return maxBsaTexturesSize;
}

QString Games::getBsaExtension()
{
    return bsaExtension;
    QLogger::QLog_Note("MainOptimizer", "ext " + bsaExtension);
}

bool Games::getHasBsaTextures()
{
    return hasBsaTextures;
}

uint Games::getMeshesUser()
{
    return meshesUser;
}

uint Games::getMeshesStream()
{
    return meshesStream;
}

NiFileVersion Games::getMeshesFileVersion()
{
    return meshesFileVersion;
}

QString Games::getTexturesFormat()
{
    return texturesFormat;
}

hkPackFormat Games::getAnimationsFormat()
{
    return animationFormat;
}


