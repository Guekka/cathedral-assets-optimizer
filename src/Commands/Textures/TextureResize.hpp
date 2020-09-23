/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/CommandBook.hpp"

namespace CAO {
class TextureResize : public Command
{
public:
    CommandType type() const override { return CommandType::Texture; };
    Priority priority() const override { return High; };
    QString name() const override { return "Resize Texture"; }

    CommandResult process(File &file) const override;
    bool isApplicable(File &file) const override;

    static DirectX::TexMetadata calculateTargetDimensions(const DirectX::TexMetadata &info,
                                                          const PatternSettings &settings);
};
REGISTER_COMMAND(TextureResize)
} // namespace CAO
