/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#include "AnimationsOptimizer.h"
#include "BsaOptimizer.h"
#include "FilesystemOperations.h"
#include "MeshesOptimizer.h"
#include "PluginsOperations.h"
#include "TexturesOptimizer.h"

enum GameMode { SSE };

struct OptOptions
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

    GameMode game;
};


/*!
 * \brief Coordinates all the subclasses in order to optimize BSAs, textures, meshes and animations
 */
class MainOptimizer : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(MainOptimizer)

public:
    MainOptimizer(const OptOptions& optOptions);

    void process(const QString& file);

    void packBsa(const QString& folder);

private:

    void processBsa(const QString& file);
    void processNif(const QString& file);
    void processDds(const QString& file);
    void processHkx(const QString& file);
    void processTga(const QString& file);

    OptOptions optOptions;

    BsaOptimizer bsaOpt;
    MeshesOptimizer meshesOpt;
};
