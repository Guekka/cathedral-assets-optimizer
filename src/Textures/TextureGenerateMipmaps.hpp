/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Command.hpp"
#include "TextureFile.hpp"
#include "pch.hpp"

namespace CAO {
class TextureGenerateMipmaps final : public Command
{
public:
    TextureGenerateMipmaps()
    {
        _type = CommandType::Texture;
        _priority = Medium;
    }

    int process(File &file, const OptionsCAO &options) override;
    bool isApplicable(File &file, const OptionsCAO &options) override;

protected:
    size_t calculateOptimalMipMapsNumber(const DirectX::TexMetadata &info) const;
};
} // namespace CAO
