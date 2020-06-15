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
    QString name() override { return "BSA Extract"; }
    CommandType type() override { return CommandType::BSAFile; };
    Priority priority() override { return VeryHigh; };

    CommandResult process(File& file) override;
    bool isApplicable(File &file) override;

    static constexpr double maxBSASize = 4.0 * GigaByte;
};
REGISTER_COMMAND(BSAExtract)
} // namespace CAO
