/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <QDir>

#include "BSACreate.hpp"
#include "Commands/BSA/Utils/BSA.hpp"
#include "Commands/BSA/Utils/BSACallback.hpp"
#include "Commands/BSA/Utils/BSASplit.hpp"
#include "File/BSA/BSAFolder.hpp"
#include "Utils/Algorithms.hpp"
#include "Utils/wildcards.hpp"

namespace CAO {
CommandResult BSACreate::process(File &file) const
{
    auto *bsaFolder = file.getFile<Resources::BSAFolder>(true);
    if (!bsaFolder)
        return CommandResultFactory::getCannotCastFileResult();

    auto bsas = BSASplit::splitBSA(QDir{file.getOutputFilePath()}, currentProfile().getGeneralSettings());

    for (auto &bsa : bsas)
    {
        bsa.name(file.getOutputFilePath(), currentProfile().getGeneralSettings());

        //Checking if a bsa already exists
        if (QFile(bsa.path).exists())
            return CommandResultFactory::getFailedResult(-1, "Failed to create BSA: a BSA already exists.");

        Resources::BSAFile archive;
        archive.bsa.set_share_data(true);

        archive.saver.set_save_type(bsa.format);
        archive.saver.set_save_path(bsa.path.toStdString());

        const bool canBeCompressed = !currentProfile().getGeneralSettings().bBSADontCompress()
                                     && bsa.type != BSAType::UncompressableBsa;

        archive.bsa.set_compressed(canBeCompressed);
        const libbsarch::fs::path &rootPath = file.getOutputFilePath().toStdString();
        archive.bsa.set_dds_callback(&BSACallback, rootPath);

        try
        {
            for (const auto &fileInBSA : bsa.files)
                archive.saver.add_file(libbsarch::disk_blob(rootPath, fileInBSA.toStdString()));
        }
        catch (const std::exception &e)
        {
            return CommandResultFactory::getFailedResult(-3, e.what());
        }

        bsaFolder->bsas.emplace_back(std::move(archive));
    }
    return CommandResultFactory::getSuccessfulResult();
}

CommandState BSACreate::isApplicable(File &file) const
{
    const auto *bsaFolder = file.getFile<Resources::BSAFolder>();
    if (!bsaFolder)
        return CommandState::NotRequired;

    if (!currentProfile().getGeneralSettings().bBSACreate())
        return CommandState::NotRequired;

    return CommandState::Ready;
}

bool BSACreate::canBeCompressedFile(const QString &filename)
{
    return currentProfile().getFileTypes().match(currentProfile().getFileTypes().slBSAUncompressibleFiles(),
                                                 filename);
}

} // namespace CAO
