/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#include "QLogger/QLogger.h"
#include "AnimationsOptimizer.h"
#include "BsaOptimizer.h"
#include "FilesystemOperations.h"
#include "MeshesOptimizer.h"
#include "PluginsOperations.h"
#include "TexturesOptimizer.h"

enum GameMode { SSE };

enum FileType { BSA, DDS, NIF, HKX, TGA };

struct optOptions
{
    bool bBsaExtract = false;
    bool bBsaCreate = false;
    bool bBsaDeleteBackup = false;
    bool bBsaOptimizeAssets = false;
    bool bAnimationsOptimization = false;
    bool bMeshesHeadparts = false;
    bool bDryRun = false;

    int iMeshesOptimizationLevel = 0;
    int iTexturesOptimizationLevel = 0;
    int iLogLevel{};

    GameMode game{};
    FileType fileType;

    QString path{};
};


/*!
 * \brief Coordinates all the subclasses in order to optimize BSAs, textures, meshes and animations
 */
class MainOptimizer : public QObject
{
public:
    MainOptimizer();

    int mainProcess();
    /*!
    * \brief Sets the log level to value
    * \param value The value to set
    */
    void setLogLevel(const QLogger::LogLevel &value) { logLevel = value; }

private:
    QLogger::LogLevel logLevel{QLogger::LogLevel::Info};

    bool checkRequirements();
    bool parseArguments();

    void processBsa();
    void processNif();
    void processDds();
    void processHkx();
    void processTga();

    optOptions options;
};
