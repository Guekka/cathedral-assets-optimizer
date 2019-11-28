/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/CommandBook.hpp"
#include "Settings/Settings.hpp"

namespace CAO {
/*!
 * \brief Coordinates all the subclasses in order to optimize BSAs, textures, meshes and animations
 */
class MainOptimizer final : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(MainOptimizer)

public:
    explicit MainOptimizer(const Settings &optOptions);

    void process(const QString &path);
    void packBsa(const QString &folder);

protected:
    bool runCommand(Command *command, File &file);
    bool loadFile(File &file, const QString &path);
    bool saveFile(File &file, const QString &path);

private:
    void processBsa(const QString &file);
    bool processStandardFile(File &file, const QString &path, const Command::CommandType &type);

    Settings _optOptions;

    BSAFile _bsaFile;
    MeshFile _meshFile;
    TextureFile _textureFile;
    AnimationFile _animFile;
    CommandBook _commandBook;
};
} // namespace CAO
