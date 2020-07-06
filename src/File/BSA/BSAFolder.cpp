/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "BSAFolder.hpp"

namespace CAO {
int BSAFolder::loadFromDisk(const QString &filePath)
{
    if (!QFileInfo(filePath).isDir())
        return 1;

    loadHelper<BSAFolderResource>(filePath);

    return 0;
}

int BSAFolder::saveToDisk([[maybe_unused]] const QString &filePath) const
{
    auto dir = dynamic_cast<const BSAFolderResource *>(&getFile());
    if (!dir)
        return 1;

    try
    {
        for (auto &bsa : dir->bsas)
            bsa.saver.save();
    }
    catch (const std::exception &e)
    {
        PLOG_ERROR << e.what();
        return 2;
    }

    for (const auto &bsa : dir->bsas)
        for (const auto &filePath : bsa.saver.get_file_list())
            if (!QFile::remove(QString::fromStdString(filePath.path_on_disk.string())))
                return 3;

    return 0;
}

bool BSAFolder::setFile(std::unique_ptr<Resource> file, bool optimizedFile)
{
    return setFileHelper<BSAFolderResource>(std::move(file), optimizedFile);
}

} // namespace CAO
