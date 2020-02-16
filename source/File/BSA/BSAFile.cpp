/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "BSAFile.hpp"

namespace CAO {
BSAFile::BSAFile()
{
    reset();
}

int BSAFile::loadFromDisk(const QString &filePath)
{
    reset();
    auto bsaFile = static_cast<BSAFileResource *>(&*_file);
    bsaFile->load_from_disk(filePath);
    _filename = filePath;
    matchSettings();
    _optimizedCurrentFile = false;

    return 0;
}

int BSAFile::saveToDisk(const QString &filePath) const
{
    auto bsaFile = static_cast<BSAFileResource *>(&*_file);
    bsaFile->save_to_disk(filePath);

    if (filePath != _filename)
    {
        if (!QFile::copy(_filename, filePath))
            return 1;
    }
    else
        return 1;

    return 0;
}

bool BSAFile::setFile(Resource &file, bool optimizedFile)
{
    return setFileHelper<BSAFileResource>(file, optimizedFile);
}

void BSAFile::reset()
{
    resetHelper<BSAFileResource>();
}

} // namespace CAO
