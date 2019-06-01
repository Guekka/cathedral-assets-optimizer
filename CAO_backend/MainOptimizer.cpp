/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MainOptimizer.h"

MainOptimizer::MainOptimizer(const OptOptionsCAO &options) : optOptions (options),
    meshesOpt(MeshesOptimizer(optOptions.bMeshesHeadparts, options.iMeshesOptimizationLevel)) {}

void MainOptimizer::process(const QString &file)
{

    if(file.endsWith(".dds", Qt::CaseInsensitive))
        processDds(file);
    else if(file.endsWith(".nif", Qt::CaseInsensitive))
        processNif(file);
    else if(file.endsWith(".tga", Qt::CaseInsensitive))
        processTga(file);
    else if(file.endsWith(".bsa", Qt::CaseInsensitive))
        processBsa(file);
    else if(file.endsWith(".hkx", Qt::CaseInsensitive))
        processHkx(file);
    else
    {
        QLogger::QLog_Error("MainOptimizer", "Cannot process: " + file);
        QLogger::QLog_Error("Errors", "Cannot process: " + file);
    }
}

void MainOptimizer::packBsa(const QString &folder)
{
    processBsa(folder);
}

void MainOptimizer::addHeadparts(const QString& folder, bool processSubDirs)
{
    QProcess listHeadparts(this);
    listHeadparts.setProgram("resources/ListHeadParts.exe");
    QStringList headparts;

    if(!processSubDirs)
        meshesOpt.listHeadparts(folder);
    else
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
        QLogger::QLog_Note("MainOptimizer", QObject::tr("BSA found ! Extracting...(this may take a long time, do not force close the program): ") + file);
        bsaOpt.extract(file, optOptions.bBsaDeleteBackup);
    }

    if(optOptions.bBsaCreate && QDir(file).exists())
    {
        QLogger::QLog_Info("MainOptimizer", tr("Creating BSA..."));

        bsaOpt.packAll(file);
        PluginsOperations::makeDummyPlugins(file);
    }

    //TODO if(options.bBsaOptimizeAssets)
}

void MainOptimizer::processDds(const QString& file)
{
    if(optOptions.iTexturesOptimizationLevel >=2 && optOptions.bDryRun)
    {
        if(TexturesOptimizer::isCompressed(file))
            QLogger::QLog_Note("MainOptimizer", file + QObject::tr(" would be compressed to BC7"));
    }
    else if(optOptions.iTexturesOptimizationLevel >=2 && !optOptions.bDryRun)
        TexturesOptimizer::convertToBc7IfUncompressed(file);
}

void MainOptimizer::processHkx(const QString& file)
{
    if(optOptions.bAnimationsOptimization && optOptions.bDryRun)
        QLogger::QLog_Note("MainOptimizer", file + QObject::tr(" would be ported to SSE"));
    else if(optOptions.bAnimationsOptimization)
        animOpt.convert(file, HKPF_AMD64);
}

void MainOptimizer::processNif(const QString& file)
{
    //TODO Scan meshes opt.scan(file);

    if(optOptions.iMeshesOptimizationLevel >=1 && optOptions.bDryRun)
        meshesOpt.dryOptimize(file);
    else if(optOptions.iMeshesOptimizationLevel >=1 && !optOptions.bDryRun)
        meshesOpt.optimize(file);
}

void MainOptimizer::processTga(const QString &file)
{
    if(optOptions.iTexturesOptimizationLevel >=1 && optOptions.bDryRun)
        QLogger::QLog_Note("MainOptimizer", file + QObject::tr(" would be converted to DDS"));
    else if(optOptions.iTexturesOptimizationLevel >=1)
        TexturesOptimizer::convertTgaToDds(file);
}



