/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "BSAExtract.hpp"
#include "BSA/Utils/BSACallback.hpp"
#include "FilesystemOperations.hpp"

namespace CAO {
CommandResult BSAExtract::process(File &file, const OptionsCAO &options)
{
    auto bsafile = dynamic_cast<BSAFile *>(&file);
    if (!bsafile)
        return _resultFactory.getCannotCastFileResult();

    auto bsaPath = bsafile->getName();
    if (!options.bBsaDeleteBackup)
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
        PLOG_ERROR << e.what();
        PLOG_ERROR << "An error occured during the extraction of: " + bsaPath + 'n'
                          + "Please extract it manually. The BSA was not deleted.";
        return _resultFactory.getFailedResult(1,
                                              QString("Failed to extract BSA with error message: '%1'").arg(e.what()));
    }

    QFile::remove(bsaPath);
    return _resultFactory.getSuccessfulResult();
}

bool BSAExtract::isApplicable(File &file, const OptionsCAO &options)
{
    if (!options.bBsaExtract)
        return false;

    auto bsafile = dynamic_cast<BSAFile *>(&file);
    if (!bsafile)
        return false;

    return true;
}
} // namespace CAO
