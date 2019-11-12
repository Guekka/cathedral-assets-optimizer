/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "BSAFile.hpp"

namespace CAO {
BSAFile::BSAFile()
    : _file(new Qlibbsarch::BSArchiveAuto(""))
{
}

int BSAFile::loadFromDisk(const QString &filePath)
{
    _file->open(filePath);
    _filename = filePath;
    _modifiedCurrentFile = false;

    return 0;
}

int BSAFile::saveToDisk(const QString &filePath) const
{
    _file->save();

    if (filePath != _filename)
    {
        if (!QFile::copy(_filename, filePath))
            return 1;
    }
    else
        return 1;

    return 0;
}

void BSAFile::reset()
{
    _file.reset();
    _filename.clear();
    _modifiedCurrentFile = false;
}

const Qlibbsarch::BSArchiveAuto &BSAFile::getFile()
{
    return *_file;
}

} // namespace CAO
