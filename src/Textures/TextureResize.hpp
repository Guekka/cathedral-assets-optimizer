/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Command.hpp"
#include "TextureFile.hpp"
#include "pch.hpp"

namespace CAO {
class TextureResize final : public Command
{
public:
    TextureResize()
    {
        _type = CommandType::Texture;
        _priority = High;
        _name = "Resize Texture";
    }

    CommandResult process(File &file, const OptionsCAO &options) override;
    bool isApplicable(File &file, const OptionsCAO &options) override;

protected:
    DirectX::TexMetadata calculateTargetDimensions(const DirectX::TexMetadata &info, const OptionsCAO &options);
};
} // namespace CAO
