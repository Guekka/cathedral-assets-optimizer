/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "BSAFile.hpp"

namespace CAO {
BSAFile::BSAFile()
{
    _file = std::make_unique<BSAFileResource>();
}

int BSAFile::loadFromDisk(const QString &filePath)
{
    auto bsaFile = static_cast<BSAFileResource *>(&*_file);
    bsaFile->open(filePath);
    _filename = filePath;
    _optimizedCurrentFile = false;

    return 0;
}

int BSAFile::saveToDisk(const QString &filePath) const
{
    auto bsaFile = static_cast<BSAFileResource *>(&*_file);
    bsaFile->save();

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
    auto bsa = dynamic_cast<TextureResource *>(&*_file);
    if (!bsa)
        return false;

    _file.reset(&file);
    _optimizedCurrentFile = optimizedFile;
    return true;
}

bool BSAFile::setFile(std::unique_ptr<Resource> &file, bool optimizedFile)
{
    auto bsa = dynamic_cast<TextureResource *>(&*_file);
    if (!bsa)
        return false;

    _file = std::move(file);
    _optimizedCurrentFile = optimizedFile;
    return true;
}

void BSAFile::reset()
{
    _file.reset();
    _filename.clear();
    _optimizedCurrentFile = false;
}

} // namespace CAO
