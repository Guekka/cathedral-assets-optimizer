/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#ifdef GUI
#include "ui_mainWindow.h"
#endif

constexpr double GigaByte = 1024 * 1024 * 1024;
#define CAO_SET_CURRENT_GAME(game) Games::getInstance()->setGame(game);

class Games final : public QObject
{
public:
    enum GameMode
    {
        Invalid = 0,
        TES5,
        SSE,
        FO4,
        Custom
    };

    Q_ENUM(GameMode)
    //Declaring enums to Qt system
    Q_ENUM(bsa_archive_type_e)
    Q_ENUM(NiFileVersion)
    Q_ENUM(DXGI_FORMAT)

    //static getters
    static bsa_archive_type_t bsaFormat() { return getInstance()->getBsaFormat(); }
    static bsa_archive_type_t bsaTexturesFormat() { return getInstance()->getBsaTexturesFormat(); }
    static double maxBsaUncompressedSize() { return getInstance()->getBsaUncompressedMaxSize(); }
    static bool hasBsaTextures() { return getInstance()->getHasBsaTextures(); }
    static double maxBsaTexturesSize() { return getInstance()->getBsaTexturesMaxSize(); }
    static QString bsaExtension() { return getInstance()->getBsaExtension(); }
    static QString bsaSuffix() { return getInstance()->getBsaSuffix(); }
    static QString bsaTexturesSuffix() { return getInstance()->getBsaTexturesSuffix(); }

    static NiFileVersion meshesFileVersion() { return getInstance()->getMeshesFileVersion(); }
    static uint meshesStream() { return getInstance()->getMeshesStream(); }
    static uint meshesUser() { return getInstance()->getMeshesUser(); }

    static hkPackFormat animationFormat() { return getInstance()->getAnimationsFormat(); }

    static DXGI_FORMAT texturesFormat() { return getInstance()->getTexturesFormat(); }
    static bool texturesConvertTga() { return getInstance()->getTexturesConvertTga(); }

    static QList<DXGI_FORMAT> texturesUnwantedFormats() { return getInstance()->getTexturesUnwantedFormats(); }

    static bool texturesCompressInterface() { return getInstance()->getTexturesCompressInterface(); }

    static QString iniPath() { return getInstance()->getIniPath(); }
    static QString logPath() { return getInstance()->getLogPath(); }
    static QString resourcePath() { return getInstance()->getResourcePath(); }

    static Games *getInstance();
    static GameMode game() { return getInstance()->getGame(); }

    //instance getters
    [[nodiscard]] bsa_archive_type_e getBsaFormat() const;
    [[nodiscard]] bsa_archive_type_t getBsaTexturesFormat() const;
    [[nodiscard]] double getBsaUncompressedMaxSize() const;
    [[nodiscard]] double getBsaTexturesMaxSize() const;
    [[nodiscard]] QString getBsaExtension() const;
    [[nodiscard]] bool getHasBsaTextures() const;
    [[nodiscard]] QString getBsaSuffix() const;
    [[nodiscard]] QString getBsaTexturesSuffix() const;

    [[nodiscard]] NiFileVersion getMeshesFileVersion() const;
    [[nodiscard]] uint getMeshesStream() const;
    [[nodiscard]] uint getMeshesUser() const;

    [[nodiscard]] hkPackFormat getAnimationsFormat() const;

    [[nodiscard]] DXGI_FORMAT getTexturesFormat() const;
    [[nodiscard]] bool getTexturesConvertTga() const;
    [[nodiscard]] QList<DXGI_FORMAT> getTexturesUnwantedFormats() const;
    [[nodiscard]] bool getTexturesCompressInterface() const;

    [[nodiscard]] QString getIniPath() const;
    [[nodiscard]] QString getLogPath() const;
    [[nodiscard]] QString getResourcePath() const;

    void setGame(const GameMode &newGame);
    void setGame(const QString &gameString);
    [[nodiscard]] GameMode getGame() const;

    static GameMode stringToGame(const QString &string);

    void saveToIni(QSettings *settings) const;
    void readFromIni(QSettings *settings);

#ifdef GUI
    void setGame(Ui::MainWindow *ui);
    static GameMode uiToGame(Ui::MainWindow *ui);
    void saveToUi(Ui::MainWindow *ui);
    void readFromUi(Ui::MainWindow *ui);
#endif

private:
    bsa_archive_type_t _bsaFormat;
    bsa_archive_type_t _bsaTexturesFormat;
    double _maxBsaUncompressedSize;
    bool _hasBsaTextures;
    double _maxBsaTexturesSize;
    QString _bsaExtension;
    QString _bsaSuffix;
    QString _bsaTexturesSuffix;

    NiFileVersion _meshesFileVersion;
    uint _meshesStream;
    uint _meshesUser;

    hkPackFormat _animationFormat;

    DXGI_FORMAT _texturesFormat;
    bool _texturesConvertTga;
    QList<QVariant> _texturesUnwantedFormats;
    bool _texturesCompressInterface;

    QString _iniPath;
    QString _logPath;
    QString _resourcePath;

    static Games *_INSTANCE;
    GameMode _game;

    Games();
};
