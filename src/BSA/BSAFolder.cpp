/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "BSAFolder.hpp"

namespace CAO {
int BSAFolder::loadFromDisk(const QString &filePath)
{
    _file.setPath(filePath);
    _filename = filePath;
    return 0;
}

int BSAFolder::saveToDisk(const QString &filePath) const
{
    [[maybe_unused]] auto &a = filePath;
    return 0;
}

QDir BSAFolder::getFile()
{
    return _file;
}

void BSAFolder::reset()
{
    _filename.clear();
    _file.setPath("");
}
} // namespace CAO
