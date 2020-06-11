/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/CommandResult.hpp"
#include "File/File.hpp"
#include "Settings/Profiles.hpp"
#include "Settings/Settings.hpp"

namespace CAO {
class Command
{
public:
    enum class CommandType
    {
        Invalid = -1,
        Texture,
        Mesh,
        BSA,
        Animation,
        Plugin
    };

    enum Priority
    {
        VeryLow = 0,
        Low,
        Medium,
        High,
        VeryHigh
    };

    Command()                = default;
    Command(const Command &) = delete;
    Command &operator=(const Command &) = delete;

    virtual bool isApplicable(File &file)     = 0;
    virtual CommandResult process(File &file) = 0;

    CommandResult processIfApplicable(File &file);

    virtual Priority priority() = 0;
    virtual CommandType type()  = 0;
    virtual QString name()      = 0;

    virtual ~Command() = default;

protected:
    CommandResultFactory _resultFactory;
};
} // namespace CAO
