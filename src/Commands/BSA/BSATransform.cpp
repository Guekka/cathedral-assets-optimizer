/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <QFile>

#include "BSATransform.hpp"
#include "File/BSA/BSAFile.hpp"
#include "Settings/Games.hpp"
#include "Utils/BSA.hpp"
#include "Utils/ScopeGuard.hpp"

namespace CAO {

CommandResult BSATransform::process(File &file) const
{
    auto *bsaFile = file.getFile<Resources::BSAFile>();
    if (!bsaFile)
        return CommandResultFactory::getCannotCastFileResult();

    const QString &origOutputPath = file.getOutputFilePath();
    QString outputPath            = origOutputPath;

    if (file.getOutputFilePath() == file.getInputFilePath())
        outputPath.insert(outputPath.length() - 4, " - out"); //Keep extension

    file.setOutputFilePath(outputPath);

    const auto &games       = GameSettings::get(currentProfile().getGeneralSettings().eGame());
    const auto current_type = bsaFile->bsa.get_type();
    const auto type = current_type == baFO4dds ? games.eBSATexturesFormat().value_or(games.eBSAFormat())
                                               : games.eBSAFormat();

    try
    {
        PLOG_INFO << "Starting optimization of BSA content";

        libbsarch::transform_archive(bsaFile->bsa, outputPath.toStdString(), *bsaFile->callback, type);
    }
    catch (const libbsarch::exception &e)
    {
        const QString &error = QString("Failed to process BSA content with error: '%1'").arg(e.what());
        return CommandResultFactory::getFailedResult(-1, error);
    }

    QFile::remove(origOutputPath);             //Won't do anything if the file doesn't exist
    QFile::rename(outputPath, origOutputPath); //Same

    return CommandResultFactory::getSuccessfulResult();
}

CommandState BSATransform::isApplicable(File &file) const
{
    if (!currentProfile().getGeneralSettings().bBSAProcessContent())
        return CommandState::NotRequired;

    auto *bsaFile = file.getFile<Resources::BSAFile>();
    if (!bsaFile)
        return CommandState::NotRequired;

    if (!bsaFile->callback)
        return CommandState::NotRequired;

    return CommandState::Ready;
}
} // namespace CAO
