/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/CommandBook.hpp"

namespace CAO {
class TextureAddAlpha : public Command
{
public:
    bool isApplicable(File &file);
    CommandResult process(File &file);
    Priority priority() { return Medium; }
    CommandType type() { return CommandType::Texture; }
    QString name() { return "Add alpha to landscape textures"; };

protected:
    bool isLandscape(const QString &filepath) const;
    void listLandscapeTextures(const GeneralSettings &settings, FileTypes &filetypes);

    std::once_flag onceLandscapeFlag_;
};
REGISTER_COMMAND(TextureAddAlpha)
} // namespace CAO
