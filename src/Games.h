/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#ifdef GUI
#include "ui_mainWindow.h"
#endif

#define CAO_SET_CURRENT_GAME(game) Games::getInstance()->setGame(game);
#define CAO_GET_CURRENT_GAME Games::getInstance()->getGame()

#define CAO_BSA_FORMAT Games::getInstance()->GetBsaFormat()
#define CAO_BSA_TEXTURES_FORMAT Games::getInstance()->getBsaTexturesFormat()
#define CAO_BSA_EXTENSION Games::getInstance()->getBsaExtension()
#define CAO_BSA_MAX_SIZE Games::getInstance()->getBsaUncompressedMaxSize()
#define CAO_BSA_TEXTURES_MAX_SIZE Games::getInstance()->getBsaTexturesMaxSize()
#define CAO_HAS_BSA_TEXTURES Games::getInstance()->getHasBsaTextures()
#define CAO_BSA_SUFFIX Games::getInstance()->getBsaSuffix()
#define CAO_BSA_TEXTURES_SUFFIX Games::getInstance()->getBsaTexturesSuffix()


#define CAO_MESHES_FILE_VERSION Games::getInstance()->getMeshesFileVersion()
#define CAO_MESHES_STREAM Games::getInstance()->getMeshesStream()
#define CAO_MESHES_USER Games::getInstance()->getMeshesUser()

#define CAO_ANIMATIONS_FORMAT Games::getInstance()->getAnimationsFormat()

#define CAO_TEXTURES_FORMAT Games::getInstance()->getTexturesFormat()
#define CAO_TEXTURES_CONVERT_TGA Games::getInstance()->getTexturesIncompatibleExtensions()
#define CAO_TEXTURES_INCOMPATIBLE_FORMATS Games::getInstance()->getTexturesIncompatibleFormats()
#define CAO_TEXTURES_COMPRESS_INTERFACE Games::getInstance()->getTexturesCompressInterface()


#define CAO_INI_PATH Games::getInstance()->getIniPath()
#define CAO_LOG_PATH Games::getInstance()->getLogPath()
#define CAO_RESOURCES_PATH Games::getInstance()->getResourcePath()

const double GigaByte = 1024 * 1024 * 1024;

class Games : public QObject
{
public:
    enum GameMode { Invalid = 0, TES5, SSE, FO4, Custom  };
    Q_ENUM(GameMode)
    //Declaring enums to Qt system
    Q_ENUM(bsa_archive_type_e)
    Q_ENUM(NiFileVersion)
    Q_ENUM(DXGI_FORMAT)

    static Games* getInstance();

    bsa_archive_type_e GetBsaFormat() const;
    bsa_archive_type_t getBsaTexturesFormat() const;
    double getBsaUncompressedMaxSize() const;
    double getBsaTexturesMaxSize() const;
    QString getBsaExtension() const;
    bool getHasBsaTextures() const;
    QString getBsaSuffix() const;
    QString getBsaTexturesSuffix() const;

    NiFileVersion getMeshesFileVersion() const;
    uint getMeshesStream() const;
    uint getMeshesUser() const;

    hkPackFormat getAnimationsFormat() const;

    DXGI_FORMAT getTexturesFormat() const;
    bool getTexturesConvertTga() const;
    QList<DXGI_FORMAT> getTexturesIncompatibleFormats() const;

    QString getIniPath() const;
    QString getLogPath() const;
    QString getResourcePath() const;

    void setGame(const GameMode& newGame);
    void setGame(const QString& gameString);
    GameMode getGame() const;

    static GameMode stringToGame(const QString& string);

    void saveToIni(QSettings *settings);
    void readFromIni(QSettings *settings);

#ifdef GUI
    void setGame(Ui::MainWindow* ui);
    static GameMode uiToGame(Ui::MainWindow* ui);
    void saveToUi(Ui::MainWindow* ui);
    void readFromUi(Ui::MainWindow* ui);
#endif

    bool getTexturesCompressInterface() const;

private:
    bsa_archive_type_t bsaFormat;
    bsa_archive_type_t bsaTexturesFormat;
    double maxBsaUncompressedSize;
    bool hasBsaTextures;
    double maxBsaTexturesSize;
    QString bsaExtension;
    QString bsaSuffix;
    QString bsaTexturesSuffix;

    NiFileVersion meshesFileVersion;
    uint meshesStream;
    uint meshesUser;

    hkPackFormat animationFormat;

    DXGI_FORMAT texturesFormat;
    bool texturesConvertTga;
    QList<QVariant> texturesIncompatibleFormats;
    bool texturesCompressInterface;

    QString iniPath;
    QString logPath;
    QString resourcePath;

    static Games* INSTANCE;
    GameMode game;

    Games();
};

