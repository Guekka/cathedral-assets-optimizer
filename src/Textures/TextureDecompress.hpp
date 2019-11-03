/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Command.hpp"
#include "TextureFile.hpp"

namespace CAO {
class TextureDecompress final : public Command
{
public:
    TextureDecompress()
    {
        _type = CommandType::Texture;
        _priority = VeryHigh;
        _name = "Decompress Texture";
    }

    CommandResult process(File &file, const OptionsCAO &options) override;

    bool isApplicable(File &file, const OptionsCAO &options) override;
};
} // namespace CAO
