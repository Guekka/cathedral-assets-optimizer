/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "File/Meshes/MeshFile.hpp"
#include <strstream>

namespace CAO {
int MeshFile::loadFromDisk(const QString &filePath)
{
    loadHelper<MeshResource>(filePath);

    auto meshFile = static_cast<MeshResource *>(&getFile(false));
    return meshFile->Load(filePath.toStdString());
}

int MeshFile::saveToDisk(const QString &filePath) const
{
    if (!saveToDiskHelper(filePath))
        return -1;

    auto meshFile = static_cast<MeshResource *>(const_cast<Resource *>((&getFile())));
    //Same reasoning for const_cast here than for BSAFile
    return meshFile->Save(filePath.toStdString());
}

int MeshFile::loadFromMemory(const void *pSource, size_t size, const QString &fileName)
{
    loadHelper<MeshResource>(fileName);

    //We know pSource is empty, so it doesn't matter if we cast away constness. Furthermore,
    //NifFile::Load does not change the pointed contents. A simple std::istream& would be enough
    char *ptr = static_cast<char *>(const_cast<void *>(pSource));

    auto meshFile = static_cast<MeshResource *>(const_cast<Resource *>((&getFile())));

#define _SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING
    //We don't want to copy the processed memory and there is no alternative to strstream
    std::strstream stream(ptr, size);
#undef _SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING

    return meshFile->Load(stream);
}

int MeshFile::saveToMemory(std::vector<std::byte> &out) const
{
    if (!saveToMemoryHelper())
        return 1;

    auto meshFile = static_cast<MeshResource *>(const_cast<Resource *>((&getFile())));

    std::stringstream strStream;
    if (int res = meshFile->Save(strStream); res)
        return res;

    transform(strStream.str(), out.begin(), [](char byte) { return std::byte(byte); });
    return 0;
}

bool MeshFile::setFile(std::unique_ptr<Resource> file, bool optimizedFile)
{
    return setFileHelper<MeshResource>(std::move(file), optimizedFile);
}
} // namespace CAO
