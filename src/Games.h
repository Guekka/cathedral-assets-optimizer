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

class Games : public QObject
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
    static inline bsa_archive_type_t bsaFormat() { return Games::getInstance()->GetBsaFormat(); }
    static inline bsa_archive_type_t bsaTexturesFormat() { return Games::getInstance()->getBsaTexturesFormat(); }
    static inline double maxBsaUncompressedSize() { return Games::getInstance()->getBsaUncompressedMaxSize(); }
    static inline bool hasBsaTextures() { return Games::getInstance()->getHasBsaTextures(); }
    static inline double maxBsaTexturesSize() { return Games::getInstance()->getBsaTexturesMaxSize(); }
    static inline QString bsaExtension() { return Games::getInstance()->getBsaExtension(); }
    static inline QString bsaSuffix() { return Games::getInstance()->getBsaSuffix(); }
    static inline QString bsaTexturesSuffix() { return Games::getInstance()->getBsaTexturesSuffix(); }

    static inline NiFileVersion meshesFileVersion() { return Games::getInstance()->getMeshesFileVersion(); }
    static inline uint meshesStream() { return Games::getInstance()->getMeshesStream(); }
    static inline uint meshesUser() { return Games::getInstance()->getMeshesUser(); }

    static inline hkPackFormat animationFormat() { return Games::getInstance()->getAnimationsFormat(); }

    static inline DXGI_FORMAT texturesFormat() { return Games::getInstance()->getTexturesFormat(); }
    static inline bool texturesConvertTga() { return Games::getInstance()->getTexturesConvertTga(); }
    static inline QList<DXGI_FORMAT> texturesUnwantedFormats()
    {
        return Games::getInstance()->getTexturesUnwantedFormats();
    }
    static inline bool texturesCompressInterface() { return Games::getInstance()->getTexturesCompressInterface(); }

    static inline QString iniPath() { return Games::getInstance()->getIniPath(); }
    static inline QString logPath() { return Games::getInstance()->getLogPath(); }
    static inline QString resourcePath() { return Games::getInstance()->getResourcePath(); }

    static Games *getInstance();
    static inline Games::GameMode game() { return Games::getInstance()->getGame(); }

    //instance getters
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
    QList<DXGI_FORMAT> getTexturesUnwantedFormats() const;
    bool getTexturesCompressInterface() const;

    QString getIniPath() const;
    QString getLogPath() const;
    QString getResourcePath() const;

    void setGame(const GameMode &newGame);
    void setGame(const QString &gameString);
    GameMode getGame() const;

    static GameMode stringToGame(const QString &string);

    void saveToIni(QSettings *settings);
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
