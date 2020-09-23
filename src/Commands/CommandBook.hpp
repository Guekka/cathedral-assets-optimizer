/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <memory>
#include <vector>

#include "Commands/Command.hpp"
#include "Commands/CommandType.hpp"

namespace CAO {
class CommandBook final
{
public:
    static void registerCommand(std::unique_ptr<Command> command);

    static std::vector<const Command *> getCommands(CommandType type);
    static const Command *getCommand(const QString &name);

    template<typename T>
    static const Command *getCommand()
    {
        static_assert(std::is_base_of_v<Command, std::remove_pointer_t<T>>, "T must derive from CAO::Command");
        static_assert(std::is_pointer_v<T>, "T must be a pointer");

        for (const std::unique_ptr<Command> &command : commands_)
            if (auto ptr = command.get(); dynamic_cast<T>(ptr))
                return ptr;
        return nullptr;
    }

private:
    static inline std::vector<std::unique_ptr<Command>> commands_;
};

class CommandBookManager final
{
public:
    /**
	 * Registers the command with the appropriate commandbook.
	 *
	 * \param command The command to manage
	 */
    CommandBookManager(std::unique_ptr<Command> command) { CommandBook::registerCommand(std::move(command)); }
};

//! Register a Spell using a CommandBookManager
#define REGISTER_COMMAND(COMMAND) inline CommandBookManager Manager_##COMMAND(std::make_unique<COMMAND>());

} // namespace CAO
