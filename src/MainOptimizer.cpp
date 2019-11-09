/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MainOptimizer.hpp"
#include "PluginsOperations.hpp"
#include "Profiles.hpp"

namespace CAO {
MainOptimizer::MainOptimizer(const OptionsCAO &optOptions)
    : _optOptions(optOptions)
{}

void MainOptimizer::process(const QString &file)
{
    if (file.endsWith(".dds", Qt::CaseInsensitive))
        processTexture(file);
    else if (file.endsWith(".nif", Qt::CaseInsensitive))
        processNif(file);
    else if (file.endsWith(".tga") && CAO::Profiles::texturesConvertTga())
        processTexture(file);
    else if (file.endsWith(Profiles::bsaExtension(), Qt::CaseInsensitive))
        processBsa(file);
    else if (file.endsWith(".hkx", Qt::CaseInsensitive))
        processHkx(file);
    else
        PLOG_ERROR << "Cannot process: " + file;
}

void MainOptimizer::processBsa(const QString &file) const
{
    if (_optOptions.bDryRun)
        return; //TODO if "dry run" run dry run on the assets in the BSA

    if (_optOptions.bBsaExtract && QFileInfo(file).isFile())
    {
        PLOG_INFO << "BSA found ! Extracting...(this may take a long time, do not force close the program): " + file;
        _bsaOpt.extract(file, _optOptions.bBsaDeleteBackup);
    }

    //TODO if(options.bBsaOptimizeAssets)
}

void MainOptimizer::packBsa(const QString &folder)
{
        PLOG_INFO << "Creating BSA...";
        _bsaOpt.packAll(folder);
        PluginsOperations::makeDummyPlugins(folder);
}

void MainOptimizer::processTexture(const QString &file)
{
    if (!_textureFile.loadFromDisk(file))
    {
        PLOG_ERROR << "Failed to open: " << file;
        return;
    }

    for (auto &command : _commandBook.getTextureCommands())
    {
        const auto &result = command->processIfApplicable(_textureFile, _optOptions);
        if (result.processedFile)
        {
            PLOG_VERBOSE << QString("Successfully applied module '%1' while processing '%2'").arg(command->name(), file);
        }
        else if (result.errorCode)
        {
            PLOG_ERROR << QString("An error happened in module '%1' while processing '%2' with error code %3")
                              .arg(command->name(), file, QString::number(result));
        }
    }

    if (!_textureFile.modifiedCurrentFile())
        return;

    //Saving to file
    QString newName = file;
    if (_textureFile.isTGA())
        newName = newName.chopped(4) + ".dds";
    if (_textureFile.saveToDisk(newName))
    {
        PLOG_ERROR << "Failed to optimize: " + file;
        return;
    }
    else if (_textureFile.isTGA())
        QFile(file).remove();

    PLOG_INFO << "Successfully optimized " << file;
}

void MainOptimizer::processHkx(const QString &file)
{
    if (_optOptions.bDryRun)
        PLOG_INFO << file + " would be converted to the appropriate format.";
    else
        _animOpt.convert(file, CAO::Profiles::animationFormat());
}

void MainOptimizer::processNif(const QString &file)
{
    if (_meshFile.loadFromDisk(file))
    {
        PLOG_ERROR << "Cannot load mesh from disk: " << file;
    }

    for (auto command : _commandBook.getMeshCommands())
        command->processIfApplicable(_meshFile, _optOptions);

    if (_meshFile.saveToDisk(file))
    {
        PLOG_ERROR << "Cannot save mesh to disk: " << file;
    }

    PLOG_INFO << "Successfully optimized " << file;
}
} // namespace CAO
