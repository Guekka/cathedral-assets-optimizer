/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "AnimationsOptimizer.h"
#include "BsaOptimizer.h"
#include "MeshesOptimizer.h"
#include "TexturesOptimizer.h"
#include "OptionsCAO.h"

/*!
 * \brief Coordinates all the subclasses in order to optimize BSAs, textures, meshes and animations
 */
class MainOptimizer : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(MainOptimizer)

public:
    MainOptimizer(const OptionsCAO& optOptions);

    void process(const QString& file);
    void packBsa(const QString& folder);
    void addHeadparts(const QString& folder, bool processSubDirs);

private:
    void processBsa(const QString& file);
    void processNif(const QString& file);
    void processDds(const QString& file);
    void processHkx(const QString& file);
    void processTga(const QString& file);

    OptionsCAO optOptions;

    BsaOptimizer bsaOpt;
    MeshesOptimizer meshesOpt;
    AnimationsOptimizer animOpt;
    TexturesOptimizer texturesOpt;
};
