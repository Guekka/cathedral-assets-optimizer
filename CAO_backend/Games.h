/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"

#define CAO_SET_CURRENT_GAME(game) Games::getInstance()->setGame(game);
#define CAO_GET_CURRENT_GAME Games::getInstance()->getGame()

#define CAO_BSA_FORMAT Games::getInstance()->GetBsaFormat()
#define CAO_BSA_TEXTURES_FORMAT Games::getInstance()->getBsaTexturesFormat()
#define CAO_BSA_EXTENSION Games::getInstance()->getBsaExtension()
#define CAO_BSA_MAX_SIZE Games::getInstance()->getBsaMaxSize()
#define CAO_BSA_TEXTURES_MAX_SIZE Games::getInstance()->getBsaTexturesMaxSize()
#define CAO_HAS_BSA_TEXTURES Games::getInstance()->getHasBsaTextures()
#define CAO_BSA_SUFFIX Games::getInstance()->getBsaSuffix()
#define CAO_BSA_TEXTURES_SUFFIX Games::getInstance()->getBsaTexturesSuffix()


#define CAO_MESHES_FILE_VERSION Games::getInstance()->getMeshesFileVersion()
#define CAO_MESHES_STREAM Games::getInstance()->getMeshesStream()
#define CAO_MESHES_USER Games::getInstance()->getMeshesUser()

#define CAO_ANIMATIONS_FORMAT Games::getInstance()->getAnimationsFormat()

#define CAO_TEXTURES_FORMAT Games::getInstance()->getTexturesFormat()
#define CAO_TEXTURES_INCOMPATIBLE_EXTENSIONS Games::getInstance()->getTexturesIncompatibleExtensions()
#define CAO_TEXTURES_INCOMPATIBLE_FORMATS Games::getInstance()->getTexturesIncompatibleFormats()

#define CAO_INI_PATH Games::getInstance()->getIniPath()
#define CAO_LOG_PATH Games::getInstance()->getLogPath()
#define CAO_RESOURCES_PATH Games::getInstance()->getResourcePath()


enum GameMode { TES5, SSE, FO4 } ;

const int GigaByte = 1024 * 1024 * 1024;

class Games
{
private:   
    bsa_archive_type_e bsaFormat;
    bsa_archive_type_t bsaTexturesFormat;
    double maxBsaSize;
    bool hasBsaTextures;
    double maxBsaTexturesSize;
    QString bsaExtension;
    QString bsaSuffix;
    QString bsaTexturesSuffix;

    NiFileVersion meshesFileVersion;
    uint meshesStream;
    uint meshesUser;

    hkPackFormat animationFormat;

    QString texturesFormat;
    QStringList texturesIncompatibleExtensions;
    QStringList texturesIncompatibleFormats;

    QString iniPath;
    QString logPath;
    QString resourcePath;

    static Games* INSTANCE;
    GameMode game;

    Games();

public:
    static Games* getInstance();

    bsa_archive_type_e GetBsaFormat() const;
    bsa_archive_type_t getBsaTexturesFormat() const;
    double getBsaMaxSize() const;
    double getBsaTexturesMaxSize() const;
    QString getBsaExtension() const;
    bool getHasBsaTextures() const;
    QString getBsaSuffix() const;
    QString getBsaTexturesSuffix() const;

    NiFileVersion getMeshesFileVersion() const;
    uint getMeshesStream() const;
    uint getMeshesUser() const;

    hkPackFormat getAnimationsFormat() const;

    QString getTexturesFormat() const;
    QStringList getTexturesIncompatibleExtensions() const;
    QStringList getTexturesIncompatibleFormats() const;

    QString getIniPath() const;
    QString getLogPath() const;
    QString getResourcePath() const;

    void setGame(const GameMode& newGame);
    GameMode getGame() const;


};

