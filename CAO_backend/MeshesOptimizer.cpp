/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MeshesOptimizer.h"

MeshesOptimizer::MeshesOptimizer(bool processHeadparts, int optimizationLevel) :
    bMeshesHeadparts (processHeadparts),
    iMeshesOptimizationLevel (optimizationLevel)
{
    //Reading custom headparts file to add them to the list.
    //Done in the constructor since the file won't change at runtime.

    QFile customHeadpartsFile("resources/customHeadparts.txt");
    if(customHeadpartsFile.open(QIODevice::ReadOnly))
    {
        QTextStream ts(&customHeadpartsFile);
        while (!ts.atEnd())
        {
            QString readLine = ts.readLine();
            if(readLine.left(1) != "#" && !readLine.isEmpty())
                headparts << readLine;
        }
    }
    else
    {
        QLogger::QLog_Warning("MeshesOptimizer", tr("No custom headparts file found. If you haven't created one, please ignore this message."));
        QLogger::QLog_Warning("Errors", tr("No custom headparts file found. If you haven't created one, please ignore this message."));
    }
}


void MeshesOptimizer::list(const QString& folderPath) //Run NifScan on modPath. Detected meshes will be stored to a list, accorded to their types.
{
    QLogger::QLog_Note("MeshesOptimizer", tr("Listing meshes..."));

    headparts.clear();

    QDir directory(folderPath);

    //Running Nifscan and ListHeadparts to fill lists

    QProcess nifScan;
    nifScan.start("resources/NifScan.exe", QStringList { folderPath, "-fixdds" });

    if(!nifScan.waitForFinished(180000))
    {
        QLogger::QLog_Error("MeshesOptimizer", tr("Nifscan has not finished within 3 minutes. Skipping mesh optimization for this mod."));
        QLogger::QLog_Error("Errors", tr("Nifscan has not finished within 3 minutes. Skipping mesh optimization for this mod."));
    }

    QString currentFile;

    /*
    while(nifScan.canReadLine())
    {
        QString readLine = QString::fromLocal8Bit(nifScan.readLine());
        if(readLine.contains("meshes\\", Qt::CaseInsensitive) && !readLine.contains("Error", Qt::CaseInsensitive))
        {
            currentFile = QDir::cleanPath(readLine.simplified());
            if(!currentFile.contains("facegendata"))
                otherMeshes << directory.filePath(currentFile);
        }

        else if(readLine.contains("unsupported", Qt::CaseInsensitive) || readLine.contains("not supported", Qt::CaseInsensitive))
        {
            crashingMeshes << directory.filePath(currentFile);
            otherMeshes.removeAll(directory.filePath(currentFile));
        }
    }
*/
    listHeadparts(directory);

    /*QLogger::QLog_Debug("MeshesOptimizer", "Headparts list:\n\n" + headparts.join("\n") + "\n\nCrashing meshes list:\n\n"
     + crashingMeshes.join("\n") + "\n\nOther meshes list:\n\n" + otherMeshes.join("\n"));*/
}




void MeshesOptimizer::listHeadparts(const QDir& directory)
{
    QProcess listHeadparts;
    listHeadparts.start("resources/ListHeadParts.exe", QStringList() << directory.path());

    if(!listHeadparts.waitForFinished(180000))
    {
        QLogger::QLog_Error("MeshesOptimizer", tr("ListHeadparts has not finished within 3 minutes. Skipping headparts optimization for this mod."));
        QLogger::QLog_Error("Errors", tr("ListHeadparts has not finished within 3 minutes. Skipping headparts optimization for this mod."));
    }

    while(listHeadparts.canReadLine())
    {
        QString readLine = QString::fromLocal8Bit(listHeadparts.readLine()).simplified();
        headparts << QDir::cleanPath(readLine);
    }
}


void MeshesOptimizer::optimize(const QString &filePath) // Optimize the selected mesh
{
    NifFile file(filePath.toStdString());
    optOptions options;
/*
    if(iMeshesOptimizationLevel >= 1 && bMeshesHeadparts && headparts.contains(filePath, Qt::CaseInsensitive))
    {
        crashingMeshes.removeAll(filePath);
        options.bsTriShape = true;
        options.headParts = true;
        QLogger::QLog_Note("MeshesOptimizer", tr("Running NifOpt...")  + tr("Processing: ") + filePath + tr(" as an headpart due to crashing meshes option"));
    }

    else if(iMeshesOptimizationLevel >= 1 && crashingMeshes.contains(filePath, Qt::CaseInsensitive))
    {
        options.bsTriShape = true;
        QLogger::QLog_Note("MeshesOptimizer", tr("Running NifOpt...")  + tr("Processing: ") + filePath + tr(" due to crashing meshes option"));
    }

    else if(iMeshesOptimizationLevel >= 3 && otherMeshes.contains(filePath, Qt::CaseInsensitive))
    {
        options.bsTriShape = true;
        QLogger::QLog_Note("MeshesOptimizer", tr("Running NifOpt...") + tr("Processing: ") + filePath + tr(" due to all meshes option"));
    }

    else if(iMeshesOptimizationLevel >= 2 && otherMeshes.contains(filePath, Qt::CaseInsensitive))
        QLogger::QLog_Note("MeshesOptimizer", tr("Running NifOpt...")  + tr("Processing: ") + filePath + tr(" due to other meshes option"));

    else if(iMeshesOptimizationLevel >= 3)
    {
        options.bsTriShape = true;
        QLogger::QLog_Note("MeshesOptimizer", tr("Running NifOpt...")  + tr("Processing: ") + filePath + tr(" due to all meshes option"));
    }
*/
    file.OptimizeFor(options);
}


void MeshesOptimizer::dryOptimize(const QString &filePath)
{/*
    if(iMeshesOptimizationLevel >= 1 && bMeshesHeadparts && headparts.contains(filePath, Qt::CaseInsensitive))
        QLogger::QLog_Note("MeshesOptimizer", filePath + tr(" would be optimized as an headpart due to necessary optimization"));

    else if(iMeshesOptimizationLevel >= 1 && crashingMeshes.contains(filePath, Qt::CaseInsensitive))
        QLogger::QLog_Note("MeshesOptimizer", filePath + tr(" would be optimized due to necessary optimization"));

    else if(iMeshesOptimizationLevel >= 2 && otherMeshes.contains(filePath, Qt::CaseInsensitive))
        QLogger::QLog_Note("MeshesOptimizer", filePath + tr(" would be optimized due to medium optimization"));

    else if(iMeshesOptimizationLevel >= 3)
        QLogger::QLog_Note("MeshesOptimizer", filePath + tr(" would be optimized due to full optimization"));*/
}





