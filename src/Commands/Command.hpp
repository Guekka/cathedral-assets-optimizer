/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "CommandType.hpp"
#include "Commands/CommandResult.hpp"
#include "Settings/Profiles.hpp"
#include "Settings/Settings.hpp"

namespace CAO {
class File;

class Command
{
public:
    enum Priority
    {
        VeryLow = 0,
        Low,
        Medium,
        High,
        VeryHigh
    };

    Command() = default;

    Command(const Command &) = delete;
    Command(Command &&)      = default;

    Command &operator=(const Command &) = delete;
    Command &operator=(Command &&) = default;

    virtual bool isApplicable(File &file) const     = 0;
    virtual CommandResult process(File &file) const = 0;

    CommandResult processIfApplicable(File &file) const;

    virtual Priority priority() const = 0;
    virtual CommandType type() const  = 0;
    virtual QString name() const      = 0;

    virtual ~Command() = default;
};
} // namespace CAO
