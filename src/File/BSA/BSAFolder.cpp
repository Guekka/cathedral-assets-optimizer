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

int BSAFolder::loadFromMemory(const void *pSource, size_t size, const QString &fileName)
{
    return 1;
}

int BSAFolder::saveToMemory(std::vector<std::byte> &out) const
{
    return 1;
}

int BSAFolder::saveToDisk([[maybe_unused]] const QString &filePath) const
{
    if (!saveToDiskHelper(filePath))
        return 1;

    auto dir = dynamic_cast<const BSAFolderResource *>(&getFile());
    if (!dir)
        return 2;

    try
    {
        for (auto &bsa : dir->bsas)
            bsa.saver.save();
    }
    catch (const std::exception &e)
    {
        PLOG_ERROR << e.what();
        return 3;
    }

    for (const auto &bsa : dir->bsas)
        for (const auto &file : bsa.saver.get_file_list())
            if (!QFile::remove(QString::fromStdString(file.path_on_disk.string())))
                return 4;

    return 0;
}

bool BSAFolder::setFile(std::unique_ptr<Resource> file, bool optimizedFile)
{
    return setFileHelper<BSAFolderResource>(std::move(file), optimizedFile);
}

} // namespace CAO
