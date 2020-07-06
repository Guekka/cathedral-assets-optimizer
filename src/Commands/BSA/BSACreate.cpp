/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "BSACreate.hpp"
#include "Commands/BSA/Utils/BSACallback.hpp"
#include "Utils/Algorithms.hpp"
#include "Utils/wildcards.hpp"

namespace CAO {
CommandResult BSACreate::process(File &file)
{
    auto bsaFolder = dynamic_cast<const BSAFolderResource *>(&file.getFile());
    if (!bsaFolder)
        return _resultFactory.getCannotCastFileResult();

    auto bsas = BSASplit::splitBSA(*bsaFolder, currentProfile().getGeneralSettings());

    for (auto &bsa : bsas)
    {
        bsa.name(bsaFolder->path(), currentProfile().getGeneralSettings());

        //Checking if a bsa already exists
        if (QFile(bsa.path).exists())
            return _resultFactory.getFailedResult(1, "Failed to create BSA: a BSA already exists.");

        BSAFileResource archive;
        archive.bsa.set_share_data(true);

        archive.saver.set_save_type(bsa.format);
        archive.saver.set_save_path(bsa.path.toStdString());

        bool canBeCompressed = currentProfile().getGeneralSettings().bBSACompressArchive()
                               && bsa.type != BSAType::UncompressableBsa;

        archive.bsa.set_compressed(canBeCompressed);
        const libbsarch::fs::path &rootPath = bsaFolder->path().toStdString();
        archive.bsa.set_dds_callback(&BSACallback, rootPath);

        try
        {
            for (const auto &fileInBSA : bsa.files)
                archive.saver.add_file(libbsarch::disk_blob(rootPath, fileInBSA.toStdString()));

            archive.saver.save();
        }
        catch (const std::exception &e)
        {
            return _resultFactory.getFailedResult(3, e.what());
        }

        bsa.files >>= pipes::for_each([](const QString &filePath) { QFile::remove(filePath); });
    }
    return _resultFactory.getSuccessfulResult();
}

bool BSACreate::isApplicable(File &file)
{
    auto bsaFolder = dynamic_cast<const BSAFolderResource *>(&file.getFile());
    if (!bsaFolder)
        return false;

    return currentProfile().getGeneralSettings().bBSACreate();
}

bool BSACreate::canBeCompressedFile(const QString &filename)
{
    return currentProfile().getFileTypes().match(currentProfile().getFileTypes().slBSAUncompressibleFiles(),
                                                 filename);
}

} // namespace CAO
