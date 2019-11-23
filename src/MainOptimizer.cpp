/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MainOptimizer.hpp"
#include "Plugins/PluginsOperations.hpp"
#include "Settings/Profiles.hpp"

namespace CAO {
MainOptimizer::MainOptimizer(const Settings &optOptions)
    : _optOptions(optOptions)
{}

void MainOptimizer::process(const QString &file)
{
    if (file.endsWith(".dds", Qt::CaseInsensitive)
        || (file.endsWith(".tga") && _optOptions.getMandatoryValue<bool>(AdvancedSettings::bTexturesTGAConvertEnabled)))
        processStandardFile(_textureFile, file, Command::CommandType::Texture);
    else if (file.endsWith(".nif", Qt::CaseInsensitive))
        processStandardFile(_meshFile, file, Command::CommandType::Mesh);
    else if (file.endsWith(_optOptions.getMandatoryValue<QString>(AdvancedSettings::sBSASuffix), Qt::CaseInsensitive))
        processBsa(file);
    else if (file.endsWith(".hkx", Qt::CaseInsensitive))
        processHkx(file);
}

void MainOptimizer::processBsa(const QString &file)
{
    if (_optOptions.getMandatoryValue<bool>(StandardSettings::bDryRun))
        return; //TODO if "dry run" run dry run on the assets in the BSA

    PLOG_INFO << "Extracting BSA: " + file;
    if (!loadFile(_bsaFile, file))
        return;
    BSAExtract command;
    if (!runCommand(&command, _bsaFile))
        return;

    //TODO if(settings.bBsaOptimizeAssets)
}

void MainOptimizer::packBsa(const QString &folder)
{
    PLOG_INFO << "Creating BSA...";
    BSAFolder bsa;
    if (!loadFile(bsa, folder))
        return;
    BSACreate command;
    if (!runCommand(&command, bsa))
        return;

    PluginsOperations::makeDummyPlugins(folder, _optOptions);
}

void MainOptimizer::processHkx(const QString &file)
{
    if (_optOptions.getMandatoryValue<bool>(StandardSettings::bDryRun))
        PLOG_INFO << file + " would be converted to the appropriate format.";
    else
        _animOpt.convert(file, _optOptions.getMandatoryValue<hkPackFormat>(AdvancedSettings::eAnimationsFormat));
}

bool MainOptimizer::processStandardFile(File &file, const QString &path, const Command::CommandType type)
{
    if (!loadFile(file, path))
        return false;

    for (auto command : _commandBook.getCommandListByType(type))
        if (!runCommand(command, file))
            return false;

    if (!file.optimizedCurrentFile())
        return false;

    if (!saveFile(file, path))
        return false;

    PLOG_INFO << "Successfully optimized " << path;
    return true;
}

bool MainOptimizer::runCommand(Command *command, File &file)
{
    const auto &result = command->processIfApplicable(file, _optOptions);
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
        PLOG_ERROR
            << QString(
                   "Unknown error happened in command '%1'. The error code was '%2' and the error message was '%3'. "
                   "This message shouldn't be printed. If you see it, "
                   "please report it.")
                   .arg(command->name(), QString::number(result.errorCode), result.errorMessage);
    }
    return false;
}

bool MainOptimizer::loadFile(File &file, const QString &path)
{
    if (file.loadFromDisk(path))
    {
        PLOG_ERROR << "Cannot load file from disk: " << file.getName();
        return false;
    }
    return true;
}

bool MainOptimizer::saveFile(File &file, const QString &path)
{
    if (file.saveToDisk(path))
    {
        PLOG_ERROR << "Cannot save file to disk: " << file.getName();
        return false;
    }
    return true;
}

} // namespace CAO
