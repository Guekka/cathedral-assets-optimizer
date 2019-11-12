/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "BSACreate.hpp"
#include "BSA/Utils/BSACallback.hpp"
#include "BSA/Utils/BSASplit.hpp"

namespace CAO {
CommandResult BSACreate::process(File &file, const OptionsCAO &options)
{
    auto bsaFolder = dynamic_cast<BSAFolder *>(&file);
    if (!bsaFolder)
        return _resultFactory.getCannotCastFileResult();

    auto dir = bsaFolder->getFile();
    auto bsas = BSASplit::splitBSA(dir);

    for (auto bsa : bsas)
    {
        BSA::nameBsa({&bsa}, dir.path());

        auto rootPath = new QString(dir.path());

        //Checking if a bsa already exists
        if (QFile(bsa.path).exists())
            return _resultFactory.getFailedResult(1, "Failed to create BSA: a BSA already exists.");

        Qlibbsarch::BSArchiveAuto archive(*rootPath);
        archive.setShareData(true);
        archive.setCompressed(true);
        archive.setDDSCallback(&BSACallback, rootPath);

        //Detecting if BSA will contain sounds, since compressing BSA breaks sounds. Same for strings, Wrye Bash complains
        for (const auto &file : bsa.files)
        {
            if (!canBeCompressedFile(file))
            {
                archive.setCompressed(false);
                break;
            }
        }

        if (bsa.files.isEmpty())
            return _resultFactory.getFailedResult(2, "Failed to create BSA: this BSA does not contain any files.");

        try
        {
            archive.addFileFromDiskRoot(bsa.files);
            archive.create(bsa.path, bsa.format);
            archive.save();
        }
        catch (const std::exception &e)
        {
            return _resultFactory.getFailedResult(3, e.what());
        }

        for (const auto &file : bsa.files)
            QFile::remove(file);
    }
    return _resultFactory.getSuccessfulResult();
}

bool BSACreate::isApplicable(File &file, const OptionsCAO &options)
{
    if (!options.bBsaCreate)
        return false;

    auto bsaFolder = dynamic_cast<BSAFolder *>(&file);
    if (!bsaFolder)
        return false;

    return true;
}

bool BSACreate::canBeCompressedFile(const QString &filename)
{
    const bool cantBeCompressed = (filename.endsWith(".wav", Qt::CaseInsensitive)
                                   || filename.endsWith(".xwm", Qt::CaseInsensitive)
                                   || filename.contains(QRegularExpression("^.+\\.[^.]*strings$")));
    return !cantBeCompressed;
}

} // namespace CAO
