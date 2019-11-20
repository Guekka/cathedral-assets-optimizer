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
    if (file.endsWith(".dds", Qt::CaseInsensitive))
        processTexture(file);
    else if (file.endsWith(".nif", Qt::CaseInsensitive))
        processNif(file);
    else if (file.endsWith(".tga") && _optOptions.getMandatoryValue<bool>(AdvancedSettings::bTexturesTGAConvertEnabled))
        processTexture(file);
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
    auto command = _commandBook.getCommandByName("BSA Extract");
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
    auto command = _commandBook.getCommandByName("BSA Create");
    if (!runCommand(command, bsa))
        return;

    PluginsOperations::makeDummyPlugins(folder, _optOptions);
}

void MainOptimizer::processTexture(const QString &file)
{
    if (!loadFile(_textureFile, file))
        return;

    for (auto &command : _commandBook.getTextureCommands())
        if (!runCommand(command, _textureFile))
            return;

    if (!_textureFile.optimizedCurrentFile())
        return;

    if (!saveFile(_textureFile, file))
        return;

    PLOG_INFO << "Successfully optimized " << file;
}

void MainOptimizer::processHkx(const QString &file)
{
    if (_optOptions.getMandatoryValue<bool>(StandardSettings::bDryRun))
        PLOG_INFO << file + " would be converted to the appropriate format.";
    else
        _animOpt.convert(file, _optOptions.getMandatoryValue<hkPackFormat>(AdvancedSettings::eAnimationsFormat));
}

void MainOptimizer::processNif(const QString &file)
{
    if (!loadFile(_meshFile, file))
        return;

    for (auto command : _commandBook.getMeshCommands())
        if (!runCommand(command, _meshFile))
            return;

    if (!_meshFile.optimizedCurrentFile())
        return;

    if (saveFile(_meshFile, file))
        return;

    PLOG_INFO << "Successfully optimized " << file;
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
