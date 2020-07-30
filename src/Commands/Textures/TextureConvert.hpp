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
    CommandType type() const override { return CommandType::Texture; };
    Priority priority() const override { return VeryLow; };
    QString name() const override { return "Convert Texture"; }

    CommandResult process(File& file) const override;
    bool isApplicable(File &file) const override;

    static HRESULT convertWithoutCompression(const DirectX::ScratchImage &image,
                                             DirectX::ScratchImage &timage,
                                             const DXGI_FORMAT &format);

    static HRESULT convertWithCompression(const DirectX::ScratchImage &image,
                                          DirectX::ScratchImage &timage,
                                          const DXGI_FORMAT &format);

    static bool needsConvert(const File &file, const DirectX::TexMetadata &info);
};
REGISTER_COMMAND(TextureConvert)
} // namespace CAO
