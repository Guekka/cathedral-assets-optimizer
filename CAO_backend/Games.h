/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"

#define CAO_SET_CURRENT_GAME(game) Games::getInstance()->setGame(game);

#define CAO_BSA_FORMAT Games::getInstance()->GetBsaFormat()
#define CAO_BSA_EXTENSION Games::getInstance()->getBsaExtension()
#define CAO_BSA_MAX_SIZE Games::getInstance()->getBsaMaxSize()
#define CAO_BSA_TEXTURES_MAX_SIZE Games::getInstance()->getBsaTexturesMaxSize()
#define CAO_HAS_BSA_TEXTURES Games::getInstance()->getHasBsaTextures()
#define CAO_MESHES_FILE_VERSION Games::getInstance()->getMeshesFileVersion()
#define CAO_MESHES_STREAM Games::getInstance()->getMeshesStream()
#define CAO_MESHES_USER Games::getInstance()->getMeshesUser()
#define CAO_ANIMATIONS_FORMAT Games::getInstance()->getAnimationsFormat()
#define CAO_TEXTURES_FORMAT Games::getInstance()->getTexturesFormat()

enum GameMode { TES5, SSE } ;

const int GigaByte = 1024 * 1024 * 1024;

class Games
{
private:   
    bsa_archive_type_e bsaFormat;
    double maxBsaSize;
    bool hasBsaTextures;
    double maxBsaTexturesSize;
    QString bsaExtension;
    NiFileVersion meshesFileVersion;
    uint meshesStream;
    uint meshesUser;
    hkPackFormat animationFormat;
    QString texturesFormat;
    QString iniPath;

    static Games* INSTANCE;

    Games();

public:
    static Games* getInstance();

    bsa_archive_type_e GetBsaFormat();
    double getBsaMaxSize();
    double getBsaTexturesMaxSize();
    QString getBsaExtension();
    bool getHasBsaTextures();
    NiFileVersion getMeshesFileVersion();
    uint getMeshesStream();
    uint getMeshesUser();
    hkPackFormat getAnimationsFormat();
    QString getTexturesFormat();

    void setGame(const GameMode& newGame);
};

