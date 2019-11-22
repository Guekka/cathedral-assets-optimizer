/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "CommandBook.hpp"

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
    registerCommand(new BSAExtract);
    registerCommand(new BSACreate);
}

void CommandBook::registerCommand(Command *command)
{
    QVector<Command *> *correspondingVector = commandTypeToVector(command->type());

    for (int i = 0; i < correspondingVector->size(); ++i)
    {
        auto &commandInVector = correspondingVector->at(i);

        if (commandInVector->name() == command->name())
            return; //Preventing duplicate commands
        if (commandInVector->priority() <= command->priority())
        {
            correspondingVector->insert(i, command);
            break;
        }
    }
}

QVector<Command *> *CommandBook::commandTypeToVector(const Command::CommandType &type)
{
    switch (type)
    {
        case Command::CommandType::BSA: return &BSACommands;
        case Command::CommandType::Texture: return &textureCommands;
        case Command::CommandType::Mesh: return &meshCommands;
        case Command::CommandType::Animation: return &animationCommands;
        case Command::CommandType::Invalid: return nullptr;
    }
}

QVector<Command *> CommandBook::getCommandListByType(const Command::CommandType &type)
{
    return *commandTypeToVector(type);
}

Command *CommandBook::getCommandByName(const QString &name)
{
    for (const auto &vec : {BSACommands, animationCommands, meshCommands, textureCommands})
        for (const auto &command : vec)
            if (command->name() == name)
                return command;
    return nullptr;
}

} // namespace CAO
