/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "File/Meshes/MeshFile.hpp"

namespace CAO {

MeshFile::MeshFile()
{
    _file = std::make_unique<MeshResource>();
}

int MeshFile::loadFromDisk(const QString &filePath)
{
    auto meshFile = static_cast<MeshResource *>(&*_file);
    _optimizedCurrentFile = false;
    return meshFile->Load(filePath.toStdString());
}

int MeshFile::saveToDisk(const QString &filePath) const
{
    auto meshFile = static_cast<MeshResource *>(&*_file);
    return meshFile->Save(filePath.toStdString());
}

bool MeshFile::setFile(Resource &file, bool optimizedFile)
{
    auto meshFile = dynamic_cast<MeshResource *>(&file);
    if (!meshFile)
        return false;

    _file.reset(&file);
    _optimizedCurrentFile |= optimizedFile;
    return true;
}

void MeshFile::reset()
{
    _file.reset(new MeshResource);
    _optimizedCurrentFile = false;
    _filename.clear();
}

} // namespace CAO
