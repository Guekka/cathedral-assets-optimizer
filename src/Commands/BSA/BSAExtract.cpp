/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "BSAExtract.hpp"
#include "Commands/BSA/Utils/BSACallback.hpp"
#include "Utils/Filesystem.hpp"

namespace CAO {
CommandResult BSAExtract::process(File& file)
{
    auto bsafile = dynamic_cast<BSAFileResource *>(&file.getFile(true));
    if (!bsafile)
        return _resultFactory.getCannotCastFileResult();

    auto bsaPath = file.getName();
    if (!currentProfile().getGeneralSettings().bBSADeleteBackup())
        bsaPath = Filesystem::backupFile(bsaPath);

    libbsarch::convertible_string rootPath = QFileInfo(bsaPath).path();

    try
    {;
        bsafile->set_dds_callback(&BSACallback, rootPath);
        bsafile->load_from_disk(bsaPath);
        bsafile->extract_all(rootPath, false);
    }
    catch (const std::exception &e)
    {
        const QString &error = QString("Failed to extract BSA with error message: '%1'").arg(e.what());
        return _resultFactory.getFailedResult(1, error);
    }
    file.setFile(
        std::make_unique<BSAFileResource>()); //Closing the archive. We won't be able to delete the file otherwise

    if (!QFile::remove(file.getName())) //We want to remove the original file, not the backup file
        return _resultFactory.getFailedResult(2, "BSA Extract succeeded but failed to delete the extracted BSA");

    return _resultFactory.getSuccessfulResult();
}

bool BSAExtract::isApplicable(File& file)
{
    auto bsafile = dynamic_cast<const BSAFileResource *>(&file.getFile());
    if (!bsafile)
        return false;

    if (QFileInfo(file.getName()).size() > maxBSASize)
    {
        PLOG_ERROR << QString("BSA '%1' is over 4GB. It cannot be extracted and may be corrupted.")
                          .arg(file.getName());
        return false;
    }

    return true;
}
} // namespace CAO
