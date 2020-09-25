/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "File/Meshes/MeshFile.hpp"
#include "Utils/Algorithms.hpp"
#include <strstream>

namespace CAO {
int MeshFile::loadFromDisk(const QString &filePath)
{
    loadHelper<Resources::Mesh>(filePath);

    auto *meshFile = getFile<Resources::Mesh>(false);
    return meshFile->Load(filePath.toStdString());
}

int MeshFile::saveToDisk(const QString &filePath)
{
    if (!saveToDiskHelper(filePath))
        return -1;

    auto *meshFile = getFile<Resources::Mesh>(false);
    //Unfortunately, NIF library really doesn't like constness
    return meshFile->Save(filePath.toStdString());
}

int MeshFile::loadFromMemory(const void *pSource, size_t size, const QString &fileName)
{
    loadHelper<Resources::Mesh>(fileName);

    //We know pSource is empty, so it doesn't matter if we cast away constness. Furthermore,
    //NifFile::Load does not change the pointed contents. A simple std::istream& would be enough
    auto ptr = static_cast<const char *>(pSource);

    auto *meshFile = getFile<Resources::Mesh>(false);

    std::string str(ptr, size);
    std::stringstream stream(std::move(str), std::ios_base::in | std::ios_base::binary);

    int res = meshFile->Load(stream);
    if (res)
        reset();
    return res;
}

int MeshFile::saveToMemory(std::vector<std::byte> &out)
{
    if (!saveToMemoryHelper())
        return 1;

    auto *meshFile = getFile<Resources::Mesh>(false);

    std::stringstream strStream;
    if (int res = meshFile->Save(strStream); res)
        return res;

    const auto &str = strStream.str();
    out.reserve(str.size());
    transform(str, std::back_inserter(out), [](char byte) { return std::byte(byte); });
    return 0;
}

bool MeshFile::setFile(Resource&& file, bool optimizedFile)
{
    return setFileHelper<Resources::Mesh>(std::move(file), optimizedFile);
}
} // namespace CAO
