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

    bool isApplicable(File &file) const;
    CommandResult process(File &file) const;
    Priority priority() const { return Medium; }
    CommandType type() const { return CommandType::Texture; }
    QString name() const{ return "Add alpha to landscape textures"; };

    static bool isLandscape(const QString &filepath);
    static void listLandscapeTextures(const GeneralSettings &settings, FileTypes &filetypes);

    static inline CallOnce callOnceLandscape_;
};
REGISTER_COMMAND(TextureAddAlpha)
} // namespace CAO
