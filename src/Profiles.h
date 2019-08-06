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

class Profiles final : public QObject
{
public:
    size_t findProfiles(const QDir &dir);

    void loadProfile(const QString &newProfile);

    void saveToIni();

#ifdef GUI
    void loadProfile(Ui::MainWindow *ui);
    static QString uiToGame(Ui::MainWindow *ui);
    void saveToUi(Ui::MainWindow *ui);
    void readFromUi(Ui::MainWindow *ui);
#endif

    //Declaring enums to Qt system
    Q_ENUM(bsa_archive_type_e)
    Q_ENUM(NiFileVersion)
    Q_ENUM(DXGI_FORMAT)

    //static getters
    [[nodiscard]] static bsa_archive_type_t bsaFormat()
    {
        return getInstance()->getBsaFormat();
    }
    [[nodiscard]] static bsa_archive_type_t bsaTexturesFormat() { return getInstance()->getBsaTexturesFormat(); }
    [[nodiscard]] static double maxBsaUncompressedSize() { return getInstance()->getBsaUncompressedMaxSize(); }
    [[nodiscard]] static bool hasBsaTextures() { return getInstance()->getHasBsaTextures(); }
    [[nodiscard]] static double maxBsaTexturesSize() { return getInstance()->getBsaTexturesMaxSize(); }
    [[nodiscard]] static QString bsaExtension() { return getInstance()->getBsaExtension(); }
    [[nodiscard]] static QString bsaSuffix() { return getInstance()->getBsaSuffix(); }
    [[nodiscard]] static QString bsaTexturesSuffix() { return getInstance()->getBsaTexturesSuffix(); }

    [[nodiscard]] static NiFileVersion meshesFileVersion() { return getInstance()->getMeshesFileVersion(); }
    [[nodiscard]] static uint meshesStream() { return getInstance()->getMeshesStream(); }
    [[nodiscard]] static uint meshesUser() { return getInstance()->getMeshesUser(); }

    [[nodiscard]] static hkPackFormat animationFormat() { return getInstance()->getAnimationsFormat(); }

    [[nodiscard]] static DXGI_FORMAT texturesFormat() { return getInstance()->getTexturesFormat(); }
    [[nodiscard]] static bool texturesConvertTga() { return getInstance()->getTexturesConvertTga(); }

    [[nodiscard]] static QList<DXGI_FORMAT> texturesUnwantedFormats()
    {
        return getInstance()->getTexturesUnwantedFormats();
    }

    [[nodiscard]] static bool texturesCompressInterface() { return getInstance()->getTexturesCompressInterface(); }

    [[nodiscard]] static QSettings *settings() { return getInstance()->getSettings(); }
    [[nodiscard]] static QString logPath() { return getInstance()->getLogPath(); }
    [[nodiscard]] static QString resourcePath() { return getInstance()->getResourcePath(); }

    [[nodiscard]] static Profiles *getInstance();
    [[nodiscard]] static QString game() { return getInstance()->getProfile(); }

    //static setter
    static void setCurrentProfile(const QString &newProfile) { getInstance()->loadProfile(newProfile); }

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

    [[nodiscard]] QSettings *getSettings() const;
    [[nodiscard]] QString getLogPath() const;
    [[nodiscard]] QString getResourcePath() const;

    [[nodiscard]] QString getProfile() const;

private:
    void readFromIni();

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

    QSettings *_settings;
    QString _logPath;
    QString _resourcePath;

    static Profiles *_INSTANCE;

    QDir _profileDir;
    QString _currentProfile;
    QVector<QString> _profiles;

    Profiles();
};
