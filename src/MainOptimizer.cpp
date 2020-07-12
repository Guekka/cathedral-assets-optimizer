/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MainOptimizer.hpp"
#include "Commands/Plugins/PluginsOperations.hpp"
#include "Settings/Profiles.hpp"

namespace CAO {
MainOptimizer::MainOptimizer() {}

void MainOptimizer::process(File &file, bool dryRun)
{
    try
    {
        if (!loadFile(file))
            return;

        dryRun ? processDry(file) : processReal(file);
    }
    catch (const std::exception &e)
    {
        PLOG_ERROR << "An exception was caught while processing '" << file.getInputFilePath() << "'\n"
                   << "Error message was: '" << e.what() << "'\n"
                   << "You can probably assume this file is broken";
        return;
    }

    PLOG_INFO << "Successfully optimized " << file.getInputFilePath();
}

void MainOptimizer::processReal(File &file)
{
    PLOG_VERBOSE << "Processing: " << file.getInputFilePath() << '\n';
    for (auto command : _commandBook.getCommandList(file.type()))
        if (!runCommand(command, file))
            return;

    if (!file.optimizedCurrentFile())
        return;

    if (!saveFile(file))
        return;
}

void MainOptimizer::processDry(File &file)
{
    PLOG_INFO << "Processing: " << file.getInputFilePath() << '\n';
    for (auto command : _commandBook.getCommandList(file.type()))
        if (!dryRunCommand(command, file))
            return;
}

void MainOptimizer::extractBSA(File &file)
{
    if (currentProfile().getGeneralSettings().bDryRun())
        return; //TODO if "dry run" run dry run on the assets in the BSA

    if (!loadFile(file))
        return;

    PLOG_INFO << "Extracting BSA: " + file.getInputFilePath();
    auto command = _commandBook.getCommand<BSAExtract>();
    if (!runCommand(command, file))
        return;

    //TODO if(settings.bBsaOptimizeAssets)
}

void MainOptimizer::packBsa(const QString &folder)
{
    PLOG_INFO << "Creating BSA...";
    BSAFolder bsa;
    bsa.setInputFilePath(folder);

    if (!loadFile(bsa))
        return;

    auto command = _commandBook.getCommand<BSACreate>();
    if (!runCommand(command, bsa))
        return;

    if (!saveFile(bsa))
        return;

    if (currentProfile().getGeneralSettings().bBSACreateDummies())
        PluginsOperations::makeDummyPlugins(folder, currentProfile().getGeneralSettings());
}

bool MainOptimizer::runCommand(CommandPtr command, File &file)
{
    const auto &result = command->processIfApplicable(file);
    if (result.processedFile)
    {
        PLOG_VERBOSE << QString("%1: %2").arg(command->name(), "applied");
        return true;
    }
    else if (result.errorCode)
    {
        PLOG_VERBOSE << QString("%1: %2 '%3'").arg(command->name(), "error", result.errorMessage);
        return false;
    }
    else
    {
        PLOG_VERBOSE << QString("%1: %2").arg(command->name(), "unnecessary");
        return true;
    }
}

bool MainOptimizer::dryRunCommand(CommandPtr command, File &file)
{
    const auto &result = command->processIfApplicable(file);
    if (result.processedFile)
    {
        PLOG_INFO << QString("%1: %2").arg(command->name(), "would be applied");
        return true;
    }
    else if (result.errorCode)
    {
        PLOG_VERBOSE << QString("%1: %2 '%3'").arg(command->name(), "error", result.errorMessage);
        return false;
    }
    else
    {
        PLOG_VERBOSE << QString("%1: %2").arg(command->name(), "unnecessary");
        return true;
    }
}

bool MainOptimizer::loadFile(File &file)
{
    if (file.loadFromDisk())
    {
        PLOG_ERROR << "Cannot load file from disk: " << file.getInputFilePath();
        return false;
    }
    return true;
}

bool MainOptimizer::saveFile(File &file)
{
    if (file.saveToDisk())
    {
        PLOG_ERROR << "Cannot save file to disk: " << file.getOutputFilePath();
        return false;
    }
    return true;
}

} // namespace CAO
