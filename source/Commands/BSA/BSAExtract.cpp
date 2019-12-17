/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "BSAExtract.hpp"
#include "Commands/BSA/Utils/BSACallback.hpp"
#include "FilesystemOperations.hpp"

namespace CAO {
CommandResult BSAExtract::process(File &file, const Settings &settings)
{
    //TODO We are creating a new BSArchiveAuto while the file is already one
    auto bsafile = dynamic_cast<BSAFileResource *>(&file.getFile(true));
    if (!bsafile)
        return _resultFactory.getCannotCastFileResult();

    auto bsaPath = file.getName();
    if (!settings.getValue<bool>(bBsaDeleteBackup))
        bsaPath = FilesystemOperations::backupFile(bsaPath);

    auto rootPath = new QString(QFileInfo(bsaPath).path());

    try
    {
        Qlibbsarch::BSArchiveAuto archive(*rootPath);
        archive.setDDSCallback(&BSACallback, rootPath);
        archive.open(bsaPath);
        archive.extractAll(*rootPath, false);
    }
    catch (const std::exception &e)
    {
        const QString &error = QString("Failed to extract BSA with error message: '%1'").arg(e.what());
        return _resultFactory.getFailedResult(1, error);
    }
    bsafile->reset();                   //We won't be able to delete the file otherwise
    if (!QFile::remove(file.getName())) //We want to remove the original file, not the backup file
        return _resultFactory.getFailedResult(2, "BSA Extract succeeded but failed to delete the extracted BSA");

    return _resultFactory.getSuccessfulResult();
}

bool BSAExtract::isApplicable(File &file, const Settings &settings)
{
    if (!settings.getValue<bool>(bBsaExtract))
        return false;

    auto bsafile = dynamic_cast<const BSAFileResource *>(&file.getFile());
    if (!bsafile)
        return false;

    return true;
}
} // namespace CAO
