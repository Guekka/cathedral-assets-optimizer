/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "Commands/CommandBook.hpp"
#include "File/Animations/AnimationFile.hpp"

namespace CAO {
/*!
   * \brief Port an Oldrim animation to Skyrim Special Edition using the Havok library
   */
class AnimationsConvert final : public Command
{
public:
    CommandResult process(File& file) override;
    bool isApplicable(File &file) override;

    QString name() override { return "Animation Convert"; }
    CommandType type() override { return CommandType::Animation; };
    Priority priority() override { return Medium; };
};

REGISTER_COMMAND(AnimationsConvert)
} // namespace CAO
