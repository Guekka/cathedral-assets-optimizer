/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MainOptimizer.h"
#include "FilesystemOperations.h"
#include "PluginsOperations.h"
#include "TexturesOptimizer.h"
#include "Games.h"

MainOptimizer::MainOptimizer(const OptionsCAO &options) : optOptions (options),
    meshesOpt(MeshesOptimizer(optOptions.bMeshesHeadparts, options.iMeshesOptimizationLevel, options.bMeshesResave)) {}

void MainOptimizer::process(const QString &file)
{

    if(file.endsWith(".dds", Qt::CaseInsensitive))
        processDds(file);
    else if(file.endsWith(".nif", Qt::CaseInsensitive))
        processNif(file);
    else if(file.endsWith(".tga", Qt::CaseInsensitive))
        processTga(file);
    else if(file.endsWith(CAO_BSA_EXTENSION, Qt::CaseInsensitive))
        processBsa(file);
    else if(file.endsWith(".hkx", Qt::CaseInsensitive))
        processHkx(file);
    else
        PLOG_ERROR << "Cannot process: " + file;
}

void MainOptimizer::packBsa(const QString &folder)
{
    processBsa(folder);
}

void MainOptimizer::addHeadparts(const QString& folder, bool processSubDirs)
{
    meshesOpt.listHeadparts(folder);
    if(processSubDirs)
    {
        QDir dir(folder);
        for(const auto& directory : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
            meshesOpt.listHeadparts(dir.filePath(directory));
    }
}


void MainOptimizer::processBsa(const QString& file)
{
    if(optOptions.bDryRun)
        return; //TODO if "dry run" run dry run on the assets in the BSA

    if(optOptions.bBsaExtract && QFileInfo(file).isFile())
    {
        PLOG_INFO << tr("BSA found ! Extracting...(this may take a long time, do not force close the program): ") + file;
        bsaOpt.extract(file, optOptions.bBsaDeleteBackup);
    }

    if(optOptions.bBsaCreate && QDir(file).exists())
    {
        PLOG_INFO <<  tr("Creating BSA...");
        bsaOpt.packAll(file);
        PluginsOperations::makeDummyPlugins(file);
    }

    //TODO if(options.bBsaOptimizeAssets)
}

void MainOptimizer::processDds(const QString& file)
{
    if(optOptions.iTexturesOptimizationLevel == 0)
        return;

    if(optOptions.bDryRun)
    {
        switch (optOptions.iTexturesOptimizationLevel)
        {
        case 2:
            if(!TexturesOptimizer::isCompressed(file))
            {
                PLOG_INFO << file + QObject::tr(" would be compressed to BC7");
            }
        case 1:
            if(TexturesOptimizer::isIncompatible(file))
            {
                PLOG_INFO << file + QObject::tr(" is incompatible and would be converted to a compatible format");
            }
        }
    }
    else
    {
        switch (optOptions.iTexturesOptimizationLevel)
        {
        case 2:
            TexturesOptimizer::compress(file);
        case 1:
            TexturesOptimizer::convertIncompatibleTextures(file);
        }
    }
}

void MainOptimizer::processHkx(const QString& file)
{
    if(!optOptions.bAnimationsOptimization)
        return;

    if(optOptions.bAnimationsOptimization && optOptions.bDryRun)
        PLOG_INFO << file + QObject::tr(" would be ported to SSE");
    else if(optOptions.bAnimationsOptimization)
        animOpt.convert(file, CAO_ANIMATIONS_FORMAT);
}

void MainOptimizer::processNif(const QString& file)
{
    if(optOptions.iMeshesOptimizationLevel == 0)
        return;

    if(optOptions.iMeshesOptimizationLevel >=1 && optOptions.bDryRun)
        meshesOpt.dryOptimize(file);
    else if(optOptions.iMeshesOptimizationLevel >=1 && !optOptions.bDryRun)
        meshesOpt.optimize(file);
}

void MainOptimizer::processTga(const QString &file)
{
    if(optOptions.iTexturesOptimizationLevel == 0)
        return;

    if(optOptions.iTexturesOptimizationLevel >=1 && optOptions.bDryRun && TexturesOptimizer::isIncompatible(file))
        PLOG_INFO << file + QObject::tr(" would be converted to DDS");
    else if(optOptions.iTexturesOptimizationLevel >=1)
        TexturesOptimizer::convertIncompatibleTextures(file);
}



