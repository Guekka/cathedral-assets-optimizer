/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "CommandBook.hpp"

namespace CAO {

void CommandBook::registerCommand(CommandPtr command)
{
    auto &correspondingVector = getCommands(command->type());
    if (correspondingVector.empty())
    {
        correspondingVector.emplace_back(std::move(command));
        return;
    }

    for (size_t i = 0; i < correspondingVector.size(); ++i)
    {
        auto &commandInVector = correspondingVector.at(i);

        if (commandInVector->name() == command->name())
            return; //Preventing duplicate commands
        if (commandInVector->priority() <= command->priority())
        {
            correspondingVector.insert(correspondingVector.begin() + i, command);
            break;
        }
    }
}

std::vector<CommandPtr> &CommandBook::getCommands(const CommandType &type)
{
    return commands_[type];
}

std::vector<CommandPtr> CommandBook::getCommandList(const CommandType &type)
{
    return getCommands(type);
}

CommandPtr CommandBook::getCommand(const QString &name)
{
    for (const auto &[_, vec] : commands_)
        for (const auto &command : vec)
            if (command->name() == name)
                return command;
    return nullptr;
}
} // namespace CAO
