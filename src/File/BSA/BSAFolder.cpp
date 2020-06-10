/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "BSAFolder.hpp"

namespace CAO {
BSAFolder::BSAFolder()
{
    reset();
}

int BSAFolder::loadFromDisk(const QString &filePath)
{
    auto dir = static_cast<BSAFolderResource *>(&getFile(false));
    dir->setPath(filePath);
    setName(filePath);
    return 0;
}

int BSAFolder::saveToDisk([[maybe_unused]] const QString &filePath) const
{
    return 1;
}

bool BSAFolder::setFile(std::unique_ptr<Resource> file, bool optimizedFile)
{
    return setFileHelper<BSAFolderResource>(std::move(file), optimizedFile);
}

void BSAFolder::reset()
{
    resetHelper<BSAFolderResource>();
}
} // namespace CAO
