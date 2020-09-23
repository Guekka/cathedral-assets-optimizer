/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "Commands/CommandBook.hpp"

namespace CAO {
/*!
   * \brief Port an Oldrim animation to Skyrim Special Edition using the Havok library
   */
class AnimationsConvert final : public Command
{
public:
    CommandResult process(File &file) const override;
    bool isApplicable(File &file) const override;

    QString name() const override { return "Animation Convert"; }
    CommandType type() const override { return CommandType::Animation; };
    Priority priority() const override { return Medium; };
};

REGISTER_COMMAND(AnimationsConvert)
} // namespace CAO
