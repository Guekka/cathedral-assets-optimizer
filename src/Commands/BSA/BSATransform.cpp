/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "BSATransform.hpp"
#include "Settings/Games.hpp"
#include "Utils/ScopeGuard.hpp"

namespace CAO {

CommandResult BSATransform::process(File &file) const
{
    auto bsaFile = dynamic_cast<BSAFileResource *>(&file.getFile(true));
    if (!bsaFile)
        return _resultFactory.getCannotCastFileResult();

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
        return _resultFactory.getFailedResult(-1, error);
    }

    QFile::remove(origOutputPath);             //Won't do anything if the file doesn't exist
    QFile::rename(outputPath, origOutputPath); //Same

    return _resultFactory.getSuccessfulResult();
}

bool BSATransform::isApplicable(File &file) const
{
    if (!currentProfile().getGeneralSettings().bBSAProcessContent())
        return false;

    auto bsaFile = dynamic_cast<BSAFileResource *>(&file.getFile(true));
    if (!bsaFile)
        return false;

    if (!bsaFile->callback)
        return false;

    return true;
}
} // namespace CAO
