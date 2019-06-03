/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"

enum GameMode { TES5, SSE } ;

const int GigaByte = 1024 * 1024 * 1024;

namespace GameModeStructs
{

struct common
{
    static inline bsa_archive_type_e bsaFormat;
    static inline double maxBsaSize;
    static inline bool hasBsaTextures;
    static inline double maxBsaTexturesSize;
    static inline QString bsaExtension;
    static inline NiFileVersion meshesFileVersion;
    static inline uint meshesStream;
    static inline uint meshesUser;
    static inline hkPackFormat animationFormat;
    static inline QString texturesFormat;
    static inline QString iniPath;
};

struct SSE : public common
{
public:
    const bsa_archive_type_e bsaFormat = baSSE;
    const double maxBsaSize = 2 * static_cast<double>(GigaByte);
    const bool hasBsaTextures = true;
    //Using 2.37GB for max textures size since this bsa will always be compressed.
    //After some experiments, it is the maximum size to ensure the bsa will always
    //stay under the maximum size : ~2.1gb
    //TODO let the user choose the max size in the INI
    const double maxBsaTexturesSize = 2.37 * static_cast<double>(GigaByte);
    const QString bsaExtension = ".bsa";
    const NiFileVersion meshesFileVersion = V20_2_0_7;
    const uint meshesStream = 100;
    const uint meshesUser = 12;
    const hkPackFormat animationFormat = HKPF_AMD64;
    const QString texturesFormat = "BC7_UNORM";
};

struct TES5 : public common
{
    const bsa_archive_type_e bsaFormat = baFO3;
    const double maxBsaSize = 2 * static_cast<double>(GigaByte);
    const bool hasBsaTextures = false;
    const double maxBsaTexturesSize = 0;
    const QString bsaExtension = ".bsa";
    const NiFileVersion meshesFileVersion = V20_2_0_7;
    const uint meshesStream = 83;
    const uint meshesUser = 12;
    const hkPackFormat animationFormat = HKPF_WIN32;
    const QString texturesFormat = "BC3_UNORM";
};

}

class Games
{
private:
    static GameModeStructs::common game;
public:
    static bsa_archive_type_e GetBsaFormat();
    static double getBsaMaxSize();
    static double getBsaTexturesMaxSize();
    static QString getBsaExtension();
    static bool hasBsaTextures();
    static NiFileVersion getMeshesFileVersion();
    static uint getMeshesStream();
    static uint getMeshesUser();
    static hkPackFormat getAnimationsFormat();
    static QString getTexturesFormat();

    static void setGame(const GameMode& newGame);
};

