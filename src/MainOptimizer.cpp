/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MainOptimizer.h"
#include "FilesystemOperations.h"
#include "Games.h"
#include "PluginsOperations.h"
#include "TexturesOptimizer.h"

MainOptimizer::MainOptimizer(const OptionsCAO &options)
    : optOptions(options)
    , meshesOpt(MeshesOptimizer(optOptions.bMeshesHeadparts, options.iMeshesOptimizationLevel, options.bMeshesResave))
{
}

void MainOptimizer::process(const QString &file)
{
    if (file.endsWith(".dds", Qt::CaseInsensitive))
        processTexture(file, TexturesOptimizer::dds);
    else if (file.endsWith(".nif", Qt::CaseInsensitive))
        processNif(file);
    else if (file.endsWith(".tga", Qt::CaseInsensitive) && Games::texturesConvertTga())
        processTexture(file, TexturesOptimizer::tga);
    else if (file.endsWith(Games::bsaExtension(), Qt::CaseInsensitive))
        processBsa(file);
    else if (file.endsWith(".hkx", Qt::CaseInsensitive))
        processHkx(file);
    else
        PLOG_ERROR << "Cannot process: " + file;
}

void MainOptimizer::packBsa(const QString &folder)
{
    processBsa(folder);
}

void MainOptimizer::addHeadparts(const QString &folder, bool processSubDirs)
{
    meshesOpt.listHeadparts(folder);
    if (processSubDirs)
    {
        QDir dir(folder);
        for (const auto &directory : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
            meshesOpt.listHeadparts(dir.filePath(directory));
    }
}

void MainOptimizer::processBsa(const QString &file)
{
    if (optOptions.bDryRun)
        return; //TODO if "dry run" run dry run on the assets in the BSA

    if (optOptions.bBsaExtract && QFileInfo(file).isFile())
    {
        PLOG_INFO << tr("BSA found ! Extracting...(this may take a long time, do not force close the program): ")
                         + file;
        bsaOpt.extract(file, optOptions.bBsaDeleteBackup);
    }

    if (optOptions.bBsaCreate && QDir(file).exists())
    {
        PLOG_INFO << tr("Creating BSA...");
        bsaOpt.packAll(file);
        PluginsOperations::makeDummyPlugins(file);
    }

    //TODO if(options.bBsaOptimizeAssets)
}

void MainOptimizer::processTexture(const QString &file, const TexturesOptimizer::TextureType &type)
{
    const bool processTextures = optOptions.bTexturesMipmaps || optOptions.bTexturesCompress
                                 || optOptions.bTexturesNecessary || optOptions.bTexturesResizeSize
                                 || optOptions.bTexturesResizeRatio;
    if (!processTextures)
        return;

    if (!texturesOpt.open(file, type))
    {
        PLOG_ERROR << tr("Failed to open: ") << file;
        return;
    }

    //Resizing
    std::optional<size_t> width;
    std::optional<size_t> height;

    if (optOptions.bTexturesResizeRatio)
    {
        width = texturesOpt.getInfo().width / optOptions.iTexturesTargetWidthRatio;
        height = texturesOpt.getInfo().height / optOptions.iTexturesTargetHeightRatio;
    }
    else if (optOptions.bTexturesResizeSize)
    {
        width = optOptions.iTexturesTargetWidth;
        height = optOptions.iTexturesTargetHeight;
    }

    if (optOptions.bDryRun)
        texturesOpt.dryOptimize(optOptions.bTexturesNecessary,
                                optOptions.bTexturesCompress,
                                optOptions.bTexturesMipmaps,
                                width,
                                height);
    else
    {
        if (!texturesOpt.optimize(optOptions.bTexturesNecessary,
                                  optOptions.bTexturesCompress,
                                  optOptions.bTexturesMipmaps,
                                  width,
                                  height))
        {
            PLOG_ERROR << "Failed to optimize: " + file;
            return;
        }
        QString newName = file;
        if (type == TexturesOptimizer::tga)
            newName = newName.chopped(4) + ".dds";
        if (!texturesOpt.saveToFile(newName))
        {
            PLOG_ERROR << "Failed to optimize: " + file;
        }
        else if (type == TexturesOptimizer::tga)
            QFile(file).remove();
    }
}

void MainOptimizer::processHkx(const QString &file)
{
    if (!optOptions.bAnimationsOptimization)
        return;

    if (optOptions.bAnimationsOptimization && optOptions.bDryRun)
        PLOG_INFO << file + QObject::tr(" would be ported to SSE");
    else if (optOptions.bAnimationsOptimization)
        animOpt.convert(file, Games::animationFormat());
}

void MainOptimizer::processNif(const QString &file)
{
    if (optOptions.iMeshesOptimizationLevel == 0)
        return;

    if (optOptions.iMeshesOptimizationLevel >= 1 && optOptions.bDryRun)
        meshesOpt.dryOptimize(file);
    else if (optOptions.iMeshesOptimizationLevel >= 1 && !optOptions.bDryRun)
        meshesOpt.optimize(file);
}
