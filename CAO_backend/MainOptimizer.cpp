/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MainOptimizer.h"

MainOptimizer::MainOptimizer(OptOptions options) : optOptions (options) {}

void MainOptimizer::run(QString file) // Process the userPath according to all user options
{
    //Requirements

    if(file.endsWith(".dds", Qt::CaseInsensitive))
        processDds(file);
    else if(file.endsWith(".nif", Qt::CaseInsensitive))
        processNif(file);
    else if(file.endsWith(".tga", Qt::CaseInsensitive))
        processTga(file);
    else if(file.endsWith(".bsa", Qt::CaseInsensitive))
        processBsa(file);
    else if(file.endsWith("hkx", Qt::CaseInsensitive))
        processHkx(file);
    else
        return ; //TODO add error message

    return;
}



void MainOptimizer::processBsa(const QString& file)
{
    if(optOptions.bDryRun)
        return; //TODO if "dry run" run dry run on the assets in the BSA

    BsaOptimizer bsaOpt;

    if(optOptions.bBsaExtract)
    {
        QLogger::QLog_Note("MainOptimizer", tr("BSA found ! Extracting...(this may take a long time, do not force close the program): ") + file);
        bsaOpt.extract(file, optOptions.bBsaDeleteBackup);
    }

    /*TODO if(options.bBsaCreate)
    {
        QLogger::QLog_Info("MainOptimizer", tr("Creating BSA..."));

        bsaOptimizer.packAll(modpathDir);
        PluginsOperations::makeDummyPlugins(modpathDir);
    }*/

    //TODO if(options.bBsaOptimizeAssets)
}

void MainOptimizer::processDds(const QString& file)
{
    if(optOptions.iTexturesOptimizationLevel >=2 && optOptions.bDryRun)
    {
        if(TexturesOptimizer::isCompressed(file))
            QLogger::QLog_Note("MainOptimizer", file + tr(" would be compressed to BC7"));
    }
    else if(optOptions.iTexturesOptimizationLevel >=2 && !optOptions.bDryRun)
        TexturesOptimizer::convertToBc7IfUncompressed(file);
}

void MainOptimizer::processHkx(const QString& file)
{
    if(optOptions.bAnimationsOptimization && optOptions.bDryRun)
        QLogger::QLog_Note("MainOptimizer", file + tr(" would be ported to SSE"));
    else if(optOptions.bAnimationsOptimization)
        AnimationsOptimizer::optimize(file);
}

void MainOptimizer::processNif(const QString& file)
{
    MeshesOptimizer opt(optOptions.bMeshesHeadparts, optOptions.iMeshesOptimizationLevel);
    //TODO Scan meshes opt.scan(options.path);

    if(optOptions.iMeshesOptimizationLevel >=1 && optOptions.bDryRun)
        opt.dryOptimize(file);
    else if(optOptions.iMeshesOptimizationLevel >=1 && !optOptions.bDryRun)
        opt.optimize(file);
}

void MainOptimizer::processTga(const QString &file)
{
    if(optOptions.iTexturesOptimizationLevel >=1 && optOptions.bDryRun)
        QLogger::QLog_Note("MainOptimizer", file + tr(" would be converted to DDS"));
    else if(optOptions.iTexturesOptimizationLevel >=1)
        TexturesOptimizer::convertTgaToDds(file);
}



