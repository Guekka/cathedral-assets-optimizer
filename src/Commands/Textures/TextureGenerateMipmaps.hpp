/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/CommandBook.hpp"

namespace CAO {
class TextureGenerateMipmaps final : public Command
{
public:
    CommandType type() const override { return CommandType::Texture; };
    Priority priority() const override { return Low; };
    QString name() const override { return "Generate Mipmaps for Texture"; }
    bool isOptimization() const override { return true; }

    CommandResult process(File &file) const override;
    CommandState isApplicable(File &file) const override;

protected:
    size_t calculateOptimalMipMapsNumber(const DirectX::TexMetadata &info) const;
};
REGISTER_COMMAND(TextureGenerateMipmaps)
} // namespace CAO
