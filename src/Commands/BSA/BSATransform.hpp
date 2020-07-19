/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "Commands/CommandBook.hpp"

namespace CAO {
class BSATransform final : public Command
{
public:
    CommandResult process(File &file) const override;
    bool isApplicable(File &file) const override;

    Priority priority() const override { return Priority::Medium; }
    CommandType type() const override { return CommandType::BSAFile; }
    QString name() const override { return "Process BSA content"; }
};

REGISTER_COMMAND(BSATransform)
} // namespace CAO
