/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/CommandBook.hpp"
#include "File/Textures/TextureFile.hpp"
#include "pch.hpp"

namespace CAO {
class TextureGenerateMipmaps final : public Command
{
public:
    TextureGenerateMipmaps()
    {
        _type = CommandType::Texture;
        _priority = Medium;
        _name = "Generate Mipmaps for Texture";
    }

    CommandResult process(File& file) override;
    bool isApplicable(File& file) override;

protected:
    size_t calculateOptimalMipMapsNumber(const DirectX::TexMetadata &info) const;
};
REGISTER_COMMAND(TextureGenerateMipmaps)
} // namespace CAO
