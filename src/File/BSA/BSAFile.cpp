/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "BSAFile.hpp"

namespace CAO {
int BSAFile::loadFromDisk(const QString &filePath)
{
    loadHelper<Resources::BSAFile>(filePath);

    auto *bsaFile = getFile<Resources::BSAFile>(false);
    bsaFile->bsa.load(filePath.toStdString());
    return 0;
}

int BSAFile::saveToDisk(const QString &filePath)
{
    if (!saveToDiskHelper(filePath))
        return 2;

    const auto *bsaFile = getFile<Resources::BSAFile>();

    bsaFile->saver.save(filePath.toStdString(), bsaFile->saver.get_save_type());
    return 0;
}

int BSAFile::loadFromMemory(const void *pSource, size_t size, const QString &fileName)
{
    return 1;
}

int BSAFile::saveToMemory(std::vector<std::byte> &out)
{
    return 1;
}

bool BSAFile::setFile(Resource &&file, bool optimizedFile)
{
    return setFileHelper<Resources::BSAFile>(std::move(file), optimizedFile);
}

} // namespace CAO
