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
    static void create(const QString &name);
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
    [[nodiscard]] static bool bsaEnabled()
    {
        return _instance._bsaEnabled;
    }
    [[nodiscard]] static bsa_archive_type_t bsaFormat() { return _instance._bsaFormat; }
    [[nodiscard]] static bsa_archive_type_t bsaTexturesFormat() { return _instance._bsaTexturesFormat; }
    [[nodiscard]] static double maxBsaUncompressedSize() { return _instance._maxBsaUncompressedSize; }
    [[nodiscard]] static bool hasBsaTextures() { return _instance._hasBsaTextures; }
    [[nodiscard]] static double maxBsaTexturesSize() { return _instance._maxBsaTexturesSize; }
    [[nodiscard]] static QString bsaExtension() { return _instance._bsaExtension; }
    [[nodiscard]] static QString bsaSuffix() { return _instance._bsaSuffix; }
    [[nodiscard]] static QString bsaTexturesSuffix() { return _instance._bsaTexturesSuffix; }

    [[nodiscard]] static bool meshesEnabled() { return _instance._meshesEnabled; }
    [[nodiscard]] static NiFileVersion meshesFileVersion() { return _instance._meshesFileVersion; }
    [[nodiscard]] static uint meshesStream() { return _instance._meshesStream; }
    [[nodiscard]] static uint meshesUser() { return _instance._meshesUser; }

    [[nodiscard]] static bool animationsEnabled() { return _instance._animationsEnabled; }
    [[nodiscard]] static hkPackFormat animationFormat() { return _instance._animationFormat; }

    [[nodiscard]] static bool texturesEnabled() { return _instance._texturesEnabled; }
    [[nodiscard]] static DXGI_FORMAT texturesFormat() { return _instance._texturesFormat; }
    [[nodiscard]] static bool texturesConvertTga() { return _instance._texturesConvertTga; }
    [[nodiscard]] static QList<DXGI_FORMAT> texturesUnwantedFormats()
    {
        QList<DXGI_FORMAT> list;
        for (const QVariant &variant : _instance._texturesUnwantedFormats)
            list << variant.value<DXGI_FORMAT>();
        return list;
    }

    [[nodiscard]] static bool texturesCompressInterface() { return _instance._texturesCompressInterface; }

    [[nodiscard]] static QSettings *settings() { return _instance._settings; }
    [[nodiscard]] static QString logPath() { return _instance._logPath; }
    [[nodiscard]] static QString resourcePath() { return _instance._resourcePath; }

    [[nodiscard]] static Profiles &getInstance();
    [[nodiscard]] static QString currentProfile() { return _instance._currentProfile; }
    [[nodiscard]] static QString currentProfileDir()
    {
        return _instance._profileDir.absoluteFilePath(_instance.currentProfile());
    }

    //static setter
    static void setCurrentProfile(const QString &newProfile) { _instance.loadProfile(newProfile); }

private:
    void readFromIni();
    size_t findProfiles(const QDir &dir);

    bool _bsaEnabled;
    bsa_archive_type_t _bsaFormat;
    bsa_archive_type_t _bsaTexturesFormat;
    double _maxBsaUncompressedSize;
    bool _hasBsaTextures;
    double _maxBsaTexturesSize;
    QString _bsaExtension;
    QString _bsaSuffix;
    QString _bsaTexturesSuffix;

    bool _meshesEnabled;
    NiFileVersion _meshesFileVersion;
    uint _meshesStream;
    uint _meshesUser;

    bool _animationsEnabled;
    hkPackFormat _animationFormat;

    bool _texturesEnabled;
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
