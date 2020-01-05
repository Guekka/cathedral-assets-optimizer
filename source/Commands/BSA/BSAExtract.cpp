/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "BSAExtract.hpp"
#include "Commands/BSA/Utils/BSACallback.hpp"
#include "FilesystemOperations.hpp"

namespace CAO {
CommandResult BSAExtract::process(File& file)
{
    //TODO We are creating a new BSArchiveAuto while the file is already one
    auto bsafile = dynamic_cast<BSAFileResource *>(&file.getFile(true));
    if (!bsafile)
        return _resultFactory.getCannotCastFileResult();

    auto bsaPath = file.getName();
    if (!file.settings().getValue<bool>(bBsaDeleteBackup))
        bsaPath = FilesystemOperations::backupFile(bsaPath);

    libbsarch::convertible_string rootPath = QFileInfo(bsaPath).path();

    try
    {
        libbsarch::bs_archive_auto archive;
        archive.set_dds_callback(&BSACallback, rootPath);
        archive.load_from_disk(bsaPath);
        archive.extract_all(rootPath, false);
    }
    catch (const std::exception &e)
    {
        const QString &error = QString("Failed to extract BSA with error message: '%1'").arg(e.what());
        return _resultFactory.getFailedResult(1, error);
    }
    file.setFile(*new BSAFileResource); //We won't be able to delete the file otherwise
    if (!QFile::remove(file.getName())) //We want to remove the original file, not the backup file
        return _resultFactory.getFailedResult(2, "BSA Extract succeeded but failed to delete the extracted BSA");

    return _resultFactory.getSuccessfulResult();
}

bool BSAExtract::isApplicable(File& file)
{
    if (!file.settings().getValue<bool>(bBsaExtract))
        return false;

    auto bsafile = dynamic_cast<const BSAFileResource *>(&file.getFile());
    if (!bsafile)
        return false;

    return true;
}
} // namespace CAO
