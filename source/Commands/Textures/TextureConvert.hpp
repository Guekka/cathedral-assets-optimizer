/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/CommandBook.hpp"
#include "Commands/Textures/TextureCompressionDevice.hpp"
#include "File/Textures/TextureFile.hpp"

namespace CAO {
class TextureConvert final : public Command
{
public:
    TextureConvert()
    {
        _type = CommandType::Texture;
        _priority = Low;
        _name = "Convert Texture";
    }

    CommandResult process(File &file, const Settings &settings) override;
    bool isApplicable(File &file, const Settings &settings) override;

protected:
    int convertWithoutCompression(const DirectX::ScratchImage &image,
                                  DirectX::ScratchImage &timage,
                                  const DXGI_FORMAT &format);

    int convertWithCompression(const DirectX::ScratchImage &image,
                               DirectX::ScratchImage &timage,
                               const DXGI_FORMAT &format);

    bool needsConvert(const TextureResource &res, const Settings &settings);
};
REGISTER_COMMAND(TextureConvert)
} // namespace CAO
