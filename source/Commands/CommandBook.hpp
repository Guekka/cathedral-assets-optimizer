/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/Command.hpp"
#include "pch.hpp"

namespace CAO {
using CommandPtr = std::shared_ptr<Command>;
class CommandBook final
{
public:
    static void registerCommand(CommandPtr command);
    QVector<CommandPtr> getCommandList(const Command::CommandType &type);
    CommandPtr getCommand(const QString &name);

    template<typename T>
    CommandPtr getCommand()
    {
        for (const auto &vec : commands())
            for (CommandPtr command : vec)
                if (dynamic_cast<T *>(&*command))
                    return command;
        return nullptr;
    }

protected:
    static QMap<Command::CommandType, QVector<CommandPtr>> &commands();
    static QVector<CommandPtr> &commandTypeToVector(const Command::CommandType &type);
};

class CommandBookManager final
{
public:
    /**
	 * Registers the command with the appropriate commandbook.
	 *
	 * \param command The command to manage
	 */
    CommandBookManager(Command *command) { CommandBook::registerCommand(CommandPtr(command)); }

//! Register a Spell using a CommandBookManager
#define REGISTER_COMMAND(COMMAND) static CommandBookManager Manager_##COMMAND(new COMMAND);
};

} // namespace CAO
