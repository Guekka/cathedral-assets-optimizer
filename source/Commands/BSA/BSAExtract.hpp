/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/CommandBook.hpp"
#include "File/BSA/BSAFile.hpp"

namespace CAO {
class BSAExtract : public Command
{
public:
    BSAExtract()
    {
        _name = "BSA Extract";
        _type = CommandType::BSA;
        _priority = VeryHigh;
    }

    CommandResult process(File& file) override;
    bool isApplicable(File& file) override;
};
REGISTER_COMMAND(BSAExtract)
} // namespace CAO
