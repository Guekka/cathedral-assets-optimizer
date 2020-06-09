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

    std::vector<CommandPtr> getCommandList(const Command::CommandType &type);
    CommandPtr getCommand(const QString &name);

    template<typename T>
    CommandPtr getCommand()
    {
        for (const auto &[_, vec] : commands_)
            for (const auto &command : vec)
                if (std::dynamic_pointer_cast<std::remove_pointer_t<T>>(command))
                    return command;
        return nullptr;
    }

private:
    static inline std::map<Command::CommandType, std::vector<CommandPtr>> commands_;
    static std::vector<CommandPtr> &getCommands(const Command::CommandType &type);
};

class CommandBookManager final
{
public:
    /**
	 * Registers the command with the appropriate commandbook.
	 *
	 * \param command The command to manage
	 */
    CommandBookManager(CommandPtr command) { CommandBook::registerCommand(std::move(command)); }

//! Register a Spell using a CommandBookManager
#define REGISTER_COMMAND(COMMAND) inline CommandBookManager Manager_##COMMAND(std::make_shared<COMMAND>());
};

} // namespace CAO
