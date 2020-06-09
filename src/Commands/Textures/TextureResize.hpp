/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/CommandBook.hpp"
#include "File/Textures/TextureFile.hpp"
#include "pch.hpp"

namespace CAO {
class TextureResize : public Command
{
public:
    CommandType type() override { return CommandType::Texture; };
    Priority priority() override { return High; };
    QString name() override { return "Resize Texture"; }

    CommandResult process(File& file) override;
    bool isApplicable(File &file) override;

protected:
    DirectX::TexMetadata calculateTargetDimensions(const DirectX::TexMetadata &info,
                                                   const PatternSettings &settings);
};
REGISTER_COMMAND(TextureResize)
} // namespace CAO
