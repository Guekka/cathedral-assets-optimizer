/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "BSA/BSAFolder.hpp"
#include "BSA/Utils/BSA.hpp"
#include "Commands/Command.hpp"

namespace CAO {
class BSACreate : public Command
{
public:
    BSACreate()
    {
        _name = "BSA Create";
        _priority = Low;
        _type = CommandType::BSA;
    }

    CommandResult process(File &file, const OptionsCAO &options) override;
    bool isApplicable(File &file, const OptionsCAO &options) override;

protected:
    bool canBeCompressedFile(const QString &filename);
};
} // namespace CAO
