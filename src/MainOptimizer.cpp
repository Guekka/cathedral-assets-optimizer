/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MainOptimizer.hpp"
#include "Commands/Plugins/PluginsOperations.hpp"
#include "Settings/Profiles.hpp"

namespace CAO {
MainOptimizer::MainOptimizer() {}

void MainOptimizer::process(const QString &path)
{
    if (path.endsWith(".dds", Qt::CaseInsensitive) || (path.endsWith(".tga", Qt::CaseInsensitive)))
        processStandardFile(_textureFile, path, Command::CommandType::Texture);

    else if (path.endsWith(".nif", Qt::CaseInsensitive) || path.endsWith(".bto", Qt::CaseInsensitive)
             || path.endsWith(".btr", Qt::CaseInsensitive))
        processStandardFile(_meshFile, path, Command::CommandType::Mesh);

    else if (path.endsWith(currentProfile().getGeneralSettings().sBSASuffix(), Qt::CaseInsensitive))
        processBsa(path);

    else if (path.endsWith(".hkx", Qt::CaseInsensitive))
        processStandardFile(_animFile, path, Command::CommandType::Animation);
}

void MainOptimizer::processBsa(const QString &file)
{
    if (currentProfile().getGeneralSettings().bDryRun())
        return; //TODO if "dry run" run dry run on the assets in the BSA

    PLOG_INFO << "Extracting BSA: " + file;
    if (!loadFile(_bsaFile, file))
        return;
    auto command = _commandBook.getCommand<BSAExtract>();
    if (!runCommand(command, _bsaFile))
        return;

    //TODO if(settings.bBsaOptimizeAssets)
}

void MainOptimizer::packBsa(const QString &folder)
{
    PLOG_INFO << "Creating BSA...";
    BSAFolder bsa;
    if (!loadFile(bsa, folder))
        return;
    auto command = _commandBook.getCommand<BSACreate>();
    if (!runCommand(command, bsa))
        return;

    if (currentProfile().getGeneralSettings().bBSACreateDummies())
        PluginsOperations::makeDummyPlugins(folder, currentProfile().getGeneralSettings());
}

bool MainOptimizer::processStandardFile(File &file, const QString &path, const Command::CommandType &type)
{
    try
    {
        if (!loadFile(file, path))
            return false;

        for (auto command : _commandBook.getCommandList(type))
            if (!runCommand(command, file))
                return false;

        if (!file.optimizedCurrentFile())
            return false;

        if (!saveFile(file, path))
            return false;
    }
    catch (const std::exception &e)
    {
        PLOG_ERROR << "An exception was caught while processing '" << path << "'\n"
                   << "Error message was: '" << e.what() << "'\n"
                   << "You can probably assume this file is broken";
        return false;
    }

    PLOG_INFO << "Successfully optimized " << path;
    return true;
}

bool MainOptimizer::runCommand(CommandPtr command, File &file)
{
    const auto &result = command->processIfApplicable(file);
    if (result.processedFile)
    {
        PLOG_VERBOSE << QString("Successfully applied module '%1' while processing '%2'")
                            .arg(command->name(), file.getName());
        return true;
    }
    else if (result.errorCode)
    {
        PLOG_ERROR << QString("An error happened in module '%1' while processing '%2': '%3'")
                          .arg(command->name(), file.getName(), result.errorMessage);
        return false;
    }
    else
    {
        PLOG_VERBOSE << QString("Module '%1' was not applied because it was not necessary").arg(command->name());
        return true;
    }
}

bool MainOptimizer::loadFile(File &file, const QString &path)
{
    if (file.loadFromDisk(path))
    {
        PLOG_ERROR << "Cannot load file from disk: " << path;
        return false;
    }
    return true;
}

bool MainOptimizer::saveFile(File &file, const QString &path)
{
    if (file.saveToDisk(path))
    {
        PLOG_ERROR << "Cannot save file to disk: " << path;
        return false;
    }
    return true;
}

} // namespace CAO
