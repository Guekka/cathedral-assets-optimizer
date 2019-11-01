/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "File.hpp"
#include "OptionsCAO.hpp"
#include "Profiles.hpp"

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
        Animation
    };

    enum Priority
    {
        Low = 0,
        Medium,
        High,
        VeryHigh
    };

    virtual int process(File &file, const OptionsCAO &options) = 0;

    [[nodiscard]] virtual bool isApplicable(File &file, const OptionsCAO &options) = 0;
    int processIfApplicable(File &file, const OptionsCAO &options);

    Priority priority() { return _priority; }
    CommandType type() { return _type; }

    virtual ~Command() = default;

protected:
    Priority _priority = Low;
    CommandType _type = CommandType::Invalid;
};
} // namespace CAO
