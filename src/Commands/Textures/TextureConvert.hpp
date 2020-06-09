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
    CommandType type() override { return CommandType::Texture; };
    Priority priority() override { return Low; };
    QString name() override { return "Convert Texture"; }

    CommandResult process(File& file) override;
    bool isApplicable(File& file) override;

protected:
    int convertWithoutCompression(const DirectX::ScratchImage &image,
                                  DirectX::ScratchImage &timage,
                                  const DXGI_FORMAT &format);

    int convertWithCompression(const DirectX::ScratchImage &image,
                               DirectX::ScratchImage &timage,
                               const DXGI_FORMAT &format);

    bool needsConvert(const File &file, DXGI_FORMAT format);
};
REGISTER_COMMAND(TextureConvert)
} // namespace CAO
