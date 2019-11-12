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
    CommandBook();
    static void registerCommand(Command *command);

    static QVector<Command *> getTextureCommands();
    static QVector<Command *> getMeshCommands();
    static QVector<Command *> getAnimationCommands();
    static QVector<Command *> getBSACommands();
    static Command *getCommandByName(const QString &name);

protected:
    static QVector<Command *> textureCommands;
    static QVector<Command *> meshCommands;
    static QVector<Command *> animationCommands;
    static QVector<Command *> BSACommands;
};
} // namespace CAO
