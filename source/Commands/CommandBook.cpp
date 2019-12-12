/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "CommandBook.hpp"

namespace CAO {

QMap<Command::CommandType, QVector<CommandPtr>> &CommandBook::commands()
{
    static QMap<Command::CommandType, QVector<CommandPtr>> _commands = QMap<Command::CommandType, QVector<CommandPtr>>();
    return _commands;
}

void CommandBook::registerCommand(CommandPtr command)
{
    auto correspondingVector = commandTypeToVector(command->type());
    if (correspondingVector.isEmpty())
    {
        correspondingVector << command;
        return;
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

QVector<CommandPtr> &CommandBook::commandTypeToVector(const Command::CommandType &type)
{
    return commands()[type];
}

QVector<CommandPtr> CommandBook::getCommandList(const Command::CommandType &type)
{
    return commandTypeToVector(type);
}

CommandPtr CommandBook::getCommand(const QString &name)
{
    for (const auto &vec : commands())
        for (auto command : vec)
            if (command->name() == name)
                return command;
    return nullptr;
}

} // namespace CAO
