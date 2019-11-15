/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "AnimationsOptimizer.hpp"
#include "BSA/BSAFile.hpp"
#include "BSA/BSAFolder.hpp"
#include "Commands/CommandBook.hpp"
#include "Meshes/Mesh.hpp"
#include "OptionsCAO.hpp"
#include "Textures/Texture.hpp"

namespace CAO {
/*!
 * \brief Coordinates all the subclasses in order to optimize BSAs, textures, meshes and animations
 */
class MainOptimizer final : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(MainOptimizer)

public:
    explicit MainOptimizer(const OptionsCAO &optOptions);

    void process(const QString &file);
    void packBsa(const QString &folder);

protected:
    bool runCommand(Command *command, File &file);
    bool loadFile(File &file, const QString &path);
    bool saveFile(File &file, const QString &path);

private:
    void processBsa(const QString &file);
    void processNif(const QString &file);
    void processTexture(const QString &file);
    void processHkx(const QString &file);

    OptionsCAO _optOptions;

    BSAFile _bsaFile;
    MeshFile _meshFile;
    AnimationsOptimizer _animOpt;
    TextureFile _textureFile;
    CommandBook _commandBook;
};
} // namespace CAO
