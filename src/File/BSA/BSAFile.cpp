/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "BSAFile.hpp"

namespace CAO {
int BSAFile::loadFromDisk(const QString &filePath)
{
    loadHelper<BSAFileResource>(filePath);

    auto bsaFile = static_cast<BSAFileResource *>(&getFile(false));
    bsaFile->bsa.load(filePath.toStdString());
    return 0;
}

int BSAFile::saveToDisk(const QString &filePath) const
{
    if (!saveHelper(filePath))
        return 2;

    auto bsaFile = static_cast<const BSAFileResource *>(&getFile());

    bsaFile->saver.save(filePath.toStdString(), bsaFile->saver.get_save_type());
    return 0;
}

bool BSAFile::setFile(std::unique_ptr<Resource> file, bool optimizedFile)
{
    return setFileHelper<BSAFileResource>(std::move(file), optimizedFile);
}

} // namespace CAO
