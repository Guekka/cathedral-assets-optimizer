/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MainOptimizer.h"
#include "Profiles.h"
#include "PluginsOperations.h"
#include "TexturesOptimizer.h"

MainOptimizer::MainOptimizer(const OptionsCAO &optOptions)
    : _optOptions(optOptions)
    , _meshesOpt(
          MeshesOptimizer(_optOptions.bMeshesHeadparts, optOptions.iMeshesOptimizationLevel, optOptions.bMeshesResave))
{
    addHeadparts();
    addLandscapeTextures();
}

void MainOptimizer::addHeadparts()
{
    _meshesOpt.listHeadparts(_optOptions.userPath);
    if (_optOptions.mode == OptionsCAO::SeveralMods)
    {
        const QDir dir(_optOptions.userPath);
        for (const auto &directory : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
            _meshesOpt.listHeadparts(dir.filePath(directory));
    }
}

void MainOptimizer::addLandscapeTextures()
{
    _meshesOpt.listHeadparts(_optOptions.userPath);
    if (_optOptions.mode == OptionsCAO::SeveralMods)
    {
        const QDir dir(_optOptions.userPath);
        for (const auto &directory : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
            _meshesOpt.listHeadparts(dir.filePath(directory));
    }
}

void MainOptimizer::process(const QString &file)
{
    try {
        if (file.endsWith(".dds", Qt::CaseInsensitive))
            processTexture(file, TexturesOptimizer::DDS);
        else if (file.endsWith(".nif", Qt::CaseInsensitive))
            processNif(file);
        else if (file.endsWith(".tga", Qt::CaseInsensitive) &&
                 Profiles::texturesConvertTga())
            processTexture(file, TexturesOptimizer::TGA);
        else if (file.endsWith(Profiles::bsaExtension(), Qt::CaseInsensitive))
            processBsa(file);
        else if (file.endsWith(".hkx", Qt::CaseInsensitive))
            processHkx(file);
        else
            PLOG_ERROR << "Cannot process: " + file;
    } catch (const std::exception& e) {
        PLOG_ERROR << "Cannot process: " + file
                   << "\nAn exception occurred: " << e.what();
    }
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

void
MainOptimizer::packBsa(const QString& folder)
{
    if (_optOptions.bBsaCreate && QDir(folder).exists())
    {
        PLOG_INFO << "Creating BSA...";
        _bsaOpt.packAll(folder, _optOptions.bBsaLeastBSA);
        if (_optOptions.bBsaCreateDummies)
            PluginsOperations::makeDummyPlugins(folder);
    }
}

void MainOptimizer::processTexture(const QString &file, const TexturesOptimizer::TextureType &type)
{
    const bool processTextures = _optOptions.bTexturesMipmaps || _optOptions.bTexturesCompress
                                 || _optOptions.bTexturesNecessary || _optOptions.bTexturesResizeSize
                                 || _optOptions.bTexturesResizeRatio;
    if (!processTextures)
        return;

    if (!_texturesOpt.open(file, type))
    {
        PLOG_ERROR << "Failed to open: " << file;
        return;
    }

    //Resizing
    std::optional<size_t> width;
    std::optional<size_t> height;

    if (_optOptions.bTexturesResizeRatio)
    {
        width = _texturesOpt.getInfo().width / _optOptions.iTexturesTargetWidthRatio;
        height = _texturesOpt.getInfo().height / _optOptions.iTexturesTargetHeightRatio;
    }
    else if (_optOptions.bTexturesResizeSize)
    {
        width = _optOptions.iTexturesTargetWidth;
        height = _optOptions.iTexturesTargetHeight;
    }

    if (_optOptions.bDryRun)
        _texturesOpt.dryOptimize(_optOptions.bTexturesNecessary,
                                 _optOptions.bTexturesCompress,
                                 _optOptions.bTexturesMipmaps,
                                 width,
                                 height);
    else
    {
        if (!_texturesOpt.optimize(_optOptions.bTexturesNecessary,
                                   _optOptions.bTexturesCompress,
                                   _optOptions.bTexturesMipmaps,
                                   width,
                                   height))
        {
            PLOG_ERROR << "Failed to optimize: " + file;
            return;
        }

        if (type == TexturesOptimizer::DDS && !_texturesOpt.modifiedCurrentTexture)
            return; //Not saving if there wasn't any change

        //Saving to file
        QString newName = file;
        if (type == TexturesOptimizer::TGA)
            newName = newName.chopped(4) + ".dds";
        if (!_texturesOpt.saveToFile(newName))
        {
            PLOG_ERROR << "Failed to optimize: " + file;
        }
        else if (type == TexturesOptimizer::TGA)
            QFile(file).remove();
    }
}

void MainOptimizer::processHkx(const QString &file)
{
    if (!_optOptions.bAnimationsOptimization)
        return;

    if (_optOptions.bAnimationsOptimization && _optOptions.bDryRun)
        PLOG_INFO << file + " would be converted to the appropriate format.";
    else if (_optOptions.bAnimationsOptimization)
        _animOpt.convert(file, Profiles::animationFormat());
}

void MainOptimizer::processNif(const QString &file)
{
    if (_optOptions.iMeshesOptimizationLevel == 0)
        return;

    if (_optOptions.iMeshesOptimizationLevel >= 1 && _optOptions.bDryRun)
        _meshesOpt.dryOptimize(file);
    else if (_optOptions.iMeshesOptimizationLevel >= 1 && !_optOptions.bDryRun)
        _meshesOpt.optimize(file);
}
