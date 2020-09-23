/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/CommandBook.hpp"
#include "Utils/CallOnce.hpp"

namespace CAO {
class TextureAddAlpha : public Command
{
public:
    TextureAddAlpha();

    CommandState isApplicable(File &file) const override;
    CommandResult process(File &file) const override;

    Priority priority() const override { return Medium; }
    CommandType type() const override { return CommandType::Texture; }
    QString name() const override { return "Add alpha to landscape textures"; }
    bool isOptimization() const override { return true; }

    static bool isLandscape(const QString &filepath);
    static void listLandscapeTextures(const GeneralSettings &settings, FileTypes &filetypes);
};
REGISTER_COMMAND(TextureAddAlpha)
} // namespace CAO
