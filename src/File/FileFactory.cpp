/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "FileFactory.hpp"
#include "File/Animations/AnimationFile.hpp"
#include "File/BSA/BSAFile.hpp"
#include "File/Meshes/MeshFile.hpp"
#include "File/Textures/TextureFile.hpp"

namespace CAO {
std::unique_ptr<File> makeFile(const QString &path, const QString &bsaExtension)
{
    std::unique_ptr<File> file;

    if (path.endsWith(".dds", Qt::CaseInsensitive) || (path.endsWith(".tga", Qt::CaseInsensitive)))
        file = std::make_unique<TextureFile>();

    else if (path.endsWith(".nif", Qt::CaseInsensitive) || path.endsWith(".bto", Qt::CaseInsensitive)
             || path.endsWith(".btr", Qt::CaseInsensitive))
        file = std::make_unique<MeshFile>();

    else if (path.endsWith(bsaExtension, Qt::CaseInsensitive))
        file = std::make_unique<BSAFile>();

    else if (path.endsWith(".hkx", Qt::CaseInsensitive))
        file = std::make_unique<AnimationFile>();

    if (file)
        file->setName(path);

    return file;
}

} // namespace CAO
