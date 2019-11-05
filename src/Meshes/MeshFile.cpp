/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "MeshFile.hpp"

namespace CAO {

MeshFile::MeshFile()
    : _file(new NifFile)
{
}

int MeshFile::loadFromDisk(const QString &filePath)
{
    return _file->Load(filePath.toStdString());
}

int MeshFile::saveToDisk(const QString &filePath) const
{
    return _file->Save(filePath);
}

const NifFile &MeshFile::getFile() const
{
    return *_file;
}

void MeshFile::setFile(NifFile &file)
{
    _file.reset(&file);
}

void MeshFile::reset()
{
    _file.reset(new NifFile);
}

} // namespace CAO
