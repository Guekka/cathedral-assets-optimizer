/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/BSA/Utils/BSA.hpp"
#include "Commands/BSA/Utils/BSASplit.hpp"
#include "Commands/CommandBook.hpp"
#include "File/BSA/BSAFolder.hpp"

namespace CAO {
class BSACreate : public Command
{
public:
    QString name() override { return "BSA Create"; }
    Priority priority() override { return Low; };
    CommandType type() override { return CommandType::BSA; };

    CommandResult process(File& file) override;
    bool isApplicable(File& file) override;

protected:
    bool canBeCompressedFile(const QString &filename);
};
REGISTER_COMMAND(BSACreate);
} // namespace CAO
