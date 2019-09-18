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
    [[nodiscard]] static bool exists(const QString &profile);
    void loadProfile(const QString &newProfile);
    [[nodiscard]] static QStringList list();
    static void create(const QString &name, const QString &baseProfile);
    [[nodiscard]] static QFile getFile(const QString &filename);
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
    [[nodiscard]] static bool isBaseProfile()
    {
        return getInstance()._isBaseProfile;
    }

    [[nodiscard]] static bool bsaEnabled() { return getInstance()._bsaEnabled; }
    [[nodiscard]] static bsa_archive_type_t bsaFormat() { return getInstance()._bsaFormat; }
    [[nodiscard]] static bsa_archive_type_t bsaTexturesFormat() { return getInstance()._bsaTexturesFormat; }
    [[nodiscard]] static double maxBsaUncompressedSize() { return getInstance()._maxBsaUncompressedSize; }
    [[nodiscard]] static bool hasBsaTextures() { return getInstance()._hasBsaTextures; }
    [[nodiscard]] static double maxBsaTexturesSize() { return getInstance()._maxBsaTexturesSize; }
    [[nodiscard]] static QString bsaExtension() { return getInstance()._bsaExtension; }
    [[nodiscard]] static QString bsaSuffix() { return getInstance()._bsaSuffix; }
    [[nodiscard]] static QString bsaTexturesSuffix() { return getInstance()._bsaTexturesSuffix; }

    [[nodiscard]] static bool meshesEnabled() { return getInstance()._meshesEnabled; }
    [[nodiscard]] static NiFileVersion meshesFileVersion() { return getInstance()._meshesFileVersion; }
    [[nodiscard]] static uint meshesStream() { return getInstance()._meshesStream; }
    [[nodiscard]] static uint meshesUser() { return getInstance()._meshesUser; }

    [[nodiscard]] static bool animationsEnabled() { return getInstance()._animationsEnabled; }
    [[nodiscard]] static hkPackFormat animationFormat() { return getInstance()._animationFormat; }

    [[nodiscard]] static bool texturesEnabled() { return getInstance()._texturesEnabled; }
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

    [[nodiscard]] static QSettings *profileSettings() { return getInstance()._profileSettings; }
    [[nodiscard]] static QSettings *optionsSettings() { return getInstance()._optionsSettings; }
    [[nodiscard]] static QSettings *commonSettings() { return getInstance()._commonSettings; }
    [[nodiscard]] static QString logPath() { return getInstance()._logPath; }

    [[nodiscard]] static Profiles &getInstance();
    [[nodiscard]] static QString currentProfile() { return getInstance()._currentProfile; }

    //static setter
    static void setCurrentProfile(const QString &newProfile) { getInstance().loadProfile(newProfile); }

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

    QSettings *_profileSettings;
    QSettings *_optionsSettings;
    QSettings *_commonSettings;
    QString _logPath;

    QDir _profileDir;
    QString _currentProfile;
    QStringList _profiles;

    bool _isBaseProfile;

    static Profiles *_instance;

    Profiles();
    void init();
};
