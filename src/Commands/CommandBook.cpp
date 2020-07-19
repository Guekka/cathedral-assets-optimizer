/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "CommandBook.hpp"
#include "Utils/Algorithms.hpp"

namespace CAO {

void CommandBook::registerCommand(std::unique_ptr<Command> command)
{
    if (contains(commands_, command))
        return;

    insert_sorted(commands_, std::move(command), [](const auto &lhs, const auto &rhs) {
        return lhs->priority() > rhs->priority();
    });
}

std::vector<const Command *> CommandBook::getCommands(CommandType type)
{
    return commands_ | rx::filter([&type](auto &&c) { return c->type() == type; })
           | rx::transform([](auto &&c) { return static_cast<const Command *>(c.get()); }) | rx::to_vector();
}

const Command *CommandBook::getCommand(const QString &name)
{
    auto it = std::find_if(commands_.cbegin(), commands_.cend(), [&name](auto &&c) {
        return c->name() == name;
    });

    if (it == commands_.cend())
        return nullptr;

    return it->get();
}
} // namespace CAO
