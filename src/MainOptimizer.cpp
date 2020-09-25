/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MainOptimizer.hpp"
#include "BSAMemoryCallback.hpp"
#include "Commands/Animations/Animation.hpp"
#include "Commands/BSA/BSA.hpp"
#include "Commands/Meshes/Mesh.hpp"
#include "Commands/Plugins/PluginsOperations.hpp"
#include "Commands/Textures/Texture.hpp"
#include "File/BSA/BSAFolder.hpp"
#include "Settings/Games.hpp"
#include "Settings/Profiles.hpp"
#include "Settings/Settings.hpp"

namespace CAO {
void MainOptimizer::process(File &file, bool dryRun, MemoryData memoryData)
{
    emit processingFile(file.type());
    try
    {
        if (!loadFile(file, memoryData.pSource, memoryData.sourceSize))
        {
            return;
        }
        if (file.type() == CommandType::BSAFile)
        {
            if (!processBSA(file, dryRun))
                return;
        }

        else if (!(dryRun ? processDry(file) : processReal(file, memoryData.out)))
        {
            return;
        }
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

bool MainOptimizer::processReal(File &file, std::vector<std::byte> *out)
{
    PLOG_VERBOSE << "Processing: " << file.getInputFilePath();
    if (!runCommands(file))
        return false;

    if (!file.optimizedCurrentFile())
        return false;

    if (!saveFile(file, out))
        return false;

    return true;
}

bool MainOptimizer::processDry(File &file)
{
    PLOG_INFO << "Processing: " << file.getInputFilePath() << '\n';
    if (!runCommands(file, true))
        return false;

    return true;
}

bool MainOptimizer::processBSA(File &file, bool dryRun = false)
{
    auto *bsaFile = file.getFile<Resources::BSAFile>(true);
    if (!bsaFile)
        return false;

    const auto &games = GameSettings::get(currentProfile().getGeneralSettings().eGame());

    bsaFile->callback = BSAMemoryCallback(games.sBSAExtension(), *this, dryRun);

    PLOG_VERBOSE << "Processing BSA: " + file.getInputFilePath();

    if (!runCommands(file))
        return false;

    if (!file.optimizedCurrentFile())
        return false;

    return true;
}

void MainOptimizer::packBsa(const QString &folder)
{
    assert(!folder.isEmpty());

    BSAFolder bsa;
    bsa.setInputFilePath(folder);

    if (!loadFile(bsa))
        return;

    auto command = CommandBook::getCommand<BSACreate *>();
    if (!command)
    {
        PLOG_ERROR << "BSA Create command was not registered";
        return;
    }

    if (command->isApplicable(bsa) != CommandState::Ready)
        return;

    PLOG_INFO << "Creating BSA...";

    if (!runCommand(*command, bsa))
        return;

    if (!saveFile(bsa))
        return;

    if (!currentProfile().getGeneralSettings().bBSADontMakeLoaded())
        PluginsOperations::makeDummyPlugins(folder, currentProfile().getGeneralSettings());
}

bool MainOptimizer::runCommands(File &file, bool dryRun)
{
    const auto commands = CommandBook::getCommands(file.type());

    bool isReady = false;
    for (const auto command : commands)
    {
        const auto res = command->isApplicable(file);

        if ((res == CommandState::PendingPreviousSteps || res == CommandState::Ready)
            && command->isOptimization())
        {
            isReady = true;
            break;
        }
    }

    if (isReady)
        for (const auto command : commands)
            if (!runCommand(*command, file, dryRun))
                return false;

    return true;
}

bool MainOptimizer::runCommand(const Command &command, File &file, bool dryRun)
{
    const auto applicable = command.isApplicable(file);
    if (applicable == CommandState::NotRequired)
    {
        PLOG_VERBOSE << QString("%1: %2").arg(command.name(), "unnecessary");
        return true;
    }
    else if (applicable == CommandState::PendingPreviousSteps)
    { //This should not happen
        PLOG_ERROR << QString("%1: %2").arg(command.name(),
                                            "was not applied because it was pending previous steps");
        return false;
    }
    //Else : ready

    const auto result = command.process(file);

    if (!result.hasError())
    {
        const plog::Severity &sev = dryRun ? plog::Severity::info : plog::Severity::verbose;
        const QString &message    = dryRun ? "would be applied" : "applied";

        PLOG(sev) << QString("%1: %2").arg(command.name(), message);
        return true;
    }
    else
    {
        PLOG_ERROR << QString("%1: error. Code: '%2'. Message: '%3'")
                          .arg(command.name(), QString::number(result.errorCode, 16), result.errorMessage);
        return false;
    }
    return true;
}

bool MainOptimizer::loadFile(File &file, void *pSource, size_t size)
{
    if (pSource)
    {
        if (file.loadFromMemory(pSource, size, file.getInputFilePath()))
        {
            PLOG_ERROR << "Cannot load file from memory: " << file.getInputFilePath();
            return false;
        }
    }
    else
    {
        if (file.loadFromDisk())
        {
            PLOG_ERROR << "Cannot load file from disk: " << file.getInputFilePath();
            return false;
        }
    }
    return true;
}

bool MainOptimizer::saveFile(File &file, std::vector<std::byte> *out)
{
    auto error = [&file](int errCode) {
        PLOG_ERROR << QString("Cannot save file to disk: '%1'\nError code: '%2'")
                          .arg(file.getOutputFilePath(), QString::number(errCode));
    };

    if (out)
    {
        if (int err = file.saveToMemory(*out); err)
        {
            error(err);
            return false;
        }
    }
    else
    {
        if (int err = file.saveToDisk(); err)
        {
            error(err);
            return false;
        }
    }
    return true;
}

} // namespace CAO
