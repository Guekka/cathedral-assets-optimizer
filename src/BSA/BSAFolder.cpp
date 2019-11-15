/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "BSAFolder.hpp"

namespace CAO {
BSAFolder::BSAFolder()
{
    _file = std::make_unique<BSAFolderResource>();
}

int BSAFolder::loadFromDisk(const QString &filePath)
{
    auto dir = static_cast<BSAFolderResource *>(&*_file);
    dir->setPath(filePath);
    _filename = filePath;
    return 0;
}

int BSAFolder::saveToDisk(const QString &filePath) const
{
    [[maybe_unused]] auto &a = filePath;
    return 1;
}

bool BSAFolder::setFile(Resource &file, bool optimizedFile)
{
    auto dir = dynamic_cast<BSAFolderResource *>(&file);
    if (!dir)
        return false;

    _optimizedCurrentFile = optimizedFile;
    _file.reset(&file);
    return true;
}

bool BSAFolder::setFile(std::unique_ptr<Resource> &file, bool optimizedFile)
{
    auto dir = dynamic_cast<BSAFolderResource *>(&*file);
    if (!dir)
        return false;

    _optimizedCurrentFile = optimizedFile;
    _file = std::move(file);
    return true;
}

void BSAFolder::reset()
{
    auto dir = static_cast<BSAFolderResource *>(&*_file);
    _filename.clear();
    dir->setPath("");
}
} // namespace CAO
