/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/Command.hpp"
#include "pch.hpp"

namespace CAO {
class CommandBook
{
public:
    static void registerCommand(Command *command);
    QVector<Command *> getCommandList(const Command::CommandType &type);
    Command *getCommand(const QString &name);

    template<typename T>
    Command *getCommand()
    {
        for (const auto &vec : {BSACommands, animationCommands, meshCommands, textureCommands})
            for (auto command : vec)
                if (dynamic_cast<T *>(command))
                    return command;
        return nullptr;
    }

protected:
    static QVector<Command *> textureCommands;
    static QVector<Command *> meshCommands;
    static QVector<Command *> animationCommands;
    static QVector<Command *> BSACommands;

    static QVector<Command *> *commandTypeToVector(const Command::CommandType &type);
};

class CommandBookManager final
{
public:
    //! Contructor.
    /**
	 * Registers the command with the appropriate commandbook.
	 *
	 * \param command The command to manage
	 */
    CommandBookManager(Command *command) { CommandBook::registerCommand(command); }

//! Register a Spell using a Librarian
#define REGISTER_COMMAND(COMMAND) static CommandBookManager __##COMMAND##__(new COMMAND);
};

} // namespace CAO
