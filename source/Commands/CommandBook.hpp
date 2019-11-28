/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/Animations/AnimationsConvert.hpp"
#include "Commands/BSA/BSACreate.hpp"
#include "Commands/BSA/BSAExtract.hpp"
#include "Commands/BSA/Utils/BSA.hpp"
#include "Commands/Meshes/MeshConvert.hpp"
#include "Commands/Meshes/MeshRenameReferencedTextures.hpp"
#include "Commands/Textures/TextureConvert.hpp"
#include "Commands/Textures/TextureDecompress.hpp"
#include "Commands/Textures/TextureGenerateMipmaps.hpp"
#include "Commands/Textures/TextureResize.hpp"
#include "pch.hpp"

namespace CAO {
class CommandBook
{
public:
    CommandBook();
    void registerCommand(Command *command);
    QVector<Command *> getCommandListByType(const Command::CommandType &type);
    Command *getCommandByName(const QString &name);

protected:
    static QVector<Command *> textureCommands;
    static QVector<Command *> meshCommands;
    static QVector<Command *> animationCommands;
    static QVector<Command *> BSACommands;

    QVector<Command *> *commandTypeToVector(const Command::CommandType &type);
};
} // namespace CAO
