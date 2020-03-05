/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "File/Meshes/MeshFile.hpp"

namespace CAO {

MeshFile::MeshFile()
{
    reset();
}

int MeshFile::loadFromDisk(const QString &filePath)
{
    reset();
    auto meshFile = static_cast<MeshResource *>(&getFile(false));
    setName(filePath);
    return meshFile->Load(filePath.toStdString());
}

int MeshFile::saveToDisk(const QString &filePath) const
{
    auto meshFile = static_cast<MeshResource *>(const_cast<Resource *>((&getFile())));
    //Same reasoning for const_cast here than for BSAFile
    return meshFile->Save(filePath.toStdString());
}

bool MeshFile::setFile(Resource &file, bool optimizedFile)
{
    return setFileHelper<MeshResource>(file, optimizedFile);
}

void MeshFile::reset()
{
    resetHelper<MeshResource>();
}

} // namespace CAO
