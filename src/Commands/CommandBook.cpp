/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "CommandBook.hpp"
#include "Meshes/Mesh.hpp"
#include "Textures/Texture.hpp"

namespace CAO {
QVector<Command *> CommandBook::BSACommands = QVector<Command *>();
QVector<Command *> CommandBook::textureCommands = QVector<Command *>();
QVector<Command *> CommandBook::meshCommands = QVector<Command *>();
QVector<Command *> CommandBook::animationCommands = QVector<Command *>();

CommandBook::CommandBook()
{
    registerCommand(new TextureResize);
    registerCommand(new TextureConvert);
    registerCommand(new TextureDecompress);
    registerCommand(new TextureGenerateMipmaps);
    registerCommand(new MeshConvert);
    registerCommand(new MeshRenameReferencedTextures);
}

void CommandBook::registerCommand(Command *command)
{
    QVector<Command *> &correspondingVector = BSACommands;
    switch (command->type())
    {
        case Command::CommandType::BSA: correspondingVector = BSACommands; break;
        case Command::CommandType::Texture: correspondingVector = textureCommands; break;
        case Command::CommandType::Mesh: correspondingVector = meshCommands; break;
        case Command::CommandType::Animation: correspondingVector = animationCommands; break;
        case Command::CommandType::Invalid: return;
    }

    for (int i = 0; i < correspondingVector.size(); ++i)
    {
        auto &commandInVector = correspondingVector.at(i);

        if (commandInVector->name() == command->name())
            return; //Preventing duplicate commands
        if (commandInVector->priority() <= command->priority())
        {
            correspondingVector.insert(i, command);
            break;
        }
    }
}

QVector<Command *> CommandBook::getTextureCommands()
{
    return textureCommands;
}

QVector<Command *> CommandBook::getMeshCommands()
{
    return meshCommands;
}

QVector<Command *> CommandBook::getAnimationCommands()
{
    return animationCommands;
}

QVector<Command *> CommandBook::getBSACommands()
{
    return BSACommands;
}

} // namespace CAO
