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
    static bool exists(const QString &profile);
    void loadProfile(const QString &newProfile);
    static QStringList list();
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
        return getInstance()._bsaFormat;
    }
    [[nodiscard]] static bsa_archive_type_t bsaTexturesFormat() { return getInstance()._bsaTexturesFormat; }
    [[nodiscard]] static double maxBsaUncompressedSize() { return getInstance()._maxBsaUncompressedSize; }
    [[nodiscard]] static bool hasBsaTextures() { return getInstance()._hasBsaTextures; }
    [[nodiscard]] static double maxBsaTexturesSize() { return getInstance()._maxBsaTexturesSize; }
    [[nodiscard]] static QString bsaExtension() { return getInstance()._bsaExtension; }
    [[nodiscard]] static QString bsaSuffix() { return getInstance()._bsaSuffix; }
    [[nodiscard]] static QString bsaTexturesSuffix() { return getInstance()._bsaTexturesSuffix; }

    [[nodiscard]] static NiFileVersion meshesFileVersion() { return getInstance()._meshesFileVersion; }
    [[nodiscard]] static uint meshesStream() { return getInstance()._meshesStream; }
    [[nodiscard]] static uint meshesUser() { return getInstance()._meshesUser; }

    [[nodiscard]] static hkPackFormat animationFormat() { return getInstance()._animationFormat; }

    [[nodiscard]] static DXGI_FORMAT texturesFormat() { return getInstance()._texturesFormat; }
    [[nodiscard]] static bool texturesConvertTga() { return getInstance()._texturesConvertTga; }

    [[nodiscard]] static QList<DXGI_FORMAT> texturesUnwantedFormats()
    {
        QList<DXGI_FORMAT> list;
        for (const QVariant &variant : getInstance()._texturesUnwantedFormats)
            list << variant.value<DXGI_FORMAT>();
        return list;
    }

    [[nodiscard]] static bool texturesCompressInterface() { return getInstance()._texturesCompressInterface; }

    [[nodiscard]] static QSettings *settings() { return getInstance()._settings; }
    [[nodiscard]] static QString logPath() { return getInstance()._logPath; }
    [[nodiscard]] static QString resourcePath() { return getInstance()._resourcePath; }

    [[nodiscard]] static Profiles &getInstance();
    [[nodiscard]] static QString game() { return getInstance()._currentProfile; }

    //static setter
    static void setCurrentProfile(const QString &newProfile) { getInstance().loadProfile(newProfile); }

private:
    void readFromIni();
    size_t findProfiles(const QDir &dir);   

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

    QDir _profileDir;
    QString _currentProfile;
    QStringList _profiles;

    static Profiles _instance;

    Profiles();
};
