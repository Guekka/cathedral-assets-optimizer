/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "Commands/Command.hpp"
#include "File/Animations/AnimationFile.hpp"

namespace CAO {
/*!
   * \brief Port an Oldrim animation to Skyrim Special Edition using the Havok library
   */
class AnimationsConvert final : public Command
{
public:
    AnimationsConvert();
    CommandResult process(File &file, const Settings &settings) override;
    bool isApplicable(File &file, const Settings &settings) override;
};
} // namespace CAO
