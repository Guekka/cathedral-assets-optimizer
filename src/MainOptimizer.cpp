/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MainOptimizer.hpp"
#include "BSAMemoryCallback.hpp"
#include "Commands/Plugins/PluginsOperations.hpp"
#include "Settings/Profiles.hpp"

namespace CAO {
MainOptimizer::MainOptimizer() {}

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
    PLOG_VERBOSE << "Processing: " << file.getInputFilePath() << '\n';
    for (auto &command : _commandBook.getCommands(file.type()))
        if (!runCommand(*command, file))
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
    for (auto command : _commandBook.getCommands(file.type()))
        if (!dryRunCommand(*command, file))
            return false;

    return true;
}

bool MainOptimizer::processBSA(File &file, bool dryRun = false)
{
    auto bsaFile = dynamic_cast<BSAFileResource *>(&file.getFile(false));
    if (!bsaFile)
        return false;

    bsaFile->callback = BSAMemoryCallback(currentProfile().getGeneralSettings().sBSAExtension(),
                                          *this,
                                          dryRun);

    PLOG_VERBOSE << "Processing BSA: " + file.getInputFilePath();

    for (auto &command : _commandBook.getCommands(file.type()))
        if (!runCommand(*command, file))
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

    auto command = _commandBook.getCommand<BSACreate *>();
    if (!command)
    {
        PLOG_ERROR << "BSA Create command was not registered";
        return;
    }

    if (!command->isApplicable(bsa))
        return;

    PLOG_INFO << "Creating BSA...";

    if (!runCommand(*command, bsa))
        return;

    if (!saveFile(bsa))
        return;

    if (currentProfile().getGeneralSettings().bBSACreateDummies())
        PluginsOperations::makeDummyPlugins(folder, currentProfile().getGeneralSettings());
}

bool MainOptimizer::runCommand(const Command &command, File &file)
{
    const auto &result = command.processIfApplicable(file);
    if (result.processedFile)
    {
        PLOG_VERBOSE << QString("%1: %2").arg(command.name(), "applied");
        return true;
    }
    else if (result.errorCode)
    {
        PLOG_ERROR << QString("%1: %2 '%3'").arg(command.name(), "error", result.errorMessage);
        return false;
    }
    else
    {
        PLOG_VERBOSE << QString("%1: %2").arg(command.name(), "unnecessary");
        return true;
    }
}

bool MainOptimizer::dryRunCommand(const Command &command, File &file)
{
    const auto &result = command.processIfApplicable(file);
    if (result.processedFile)
    {
        PLOG_INFO << QString("%1: %2").arg(command.name(), "would be applied");
        return true;
    }
    else if (result.errorCode)
    {
        PLOG_ERROR << QString("%1: %2 '%3'").arg(command.name(), "error", result.errorMessage);
        return false;
    }
    else
    {
        PLOG_VERBOSE << QString("%1: %2").arg(command.name(), "unnecessary");
        return true;
    }
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
                          .arg(file.getOutputFilePath(), errCode);
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
