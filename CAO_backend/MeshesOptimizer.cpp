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
        QLogger::QLog_Warning("MeshesOptimizer", tr("No custom headparts file found. If you haven't created one, please ignore this message."));
}

ScanResult MeshesOptimizer::scan(const QString &filePath)
{
    NifFile nif;
    if(nif.Load(filePath.toStdString()) != 0)
    {
        QLogger::QLog_Error("MeshesOptimizer", tr("Cannot load mesh: ") + filePath);
        return doNotProcess;
    }
    ScanResult result;

    result = good;

    if(CAO_GET_CURRENT_GAME == SSE)
    {
        for(const auto& shape : nif.GetShapes())
        {
            if (shape->HasType<NiTriStrips>() || shape->HasType<bhkMultiSphereShape>())
                if(result < criticalIssue) result = criticalIssue;

            if(shape->HasType<NiParticles>() || shape->HasType<NiParticleSystem>()
                    || shape->HasType<NiParticlesData>())
            {
                return doNotProcess;
            }
        }
    }
    else if(CAO_GET_CURRENT_GAME == TES5)
        result = criticalIssue;
    else
        result = doNotProcess;

    return  result;
}

void MeshesOptimizer::listHeadparts(const QString& directory)
{
    QDirIterator it(directory, QDirIterator::Subdirectories);

    while(it.hasNext())
    {
        it.next();
        if(it.fileName().contains(QRegularExpression("\\.es[plm]$")))
            headparts += PluginsOperations::listHeadparts(it.filePath());
    }

    headparts.removeDuplicates();
}


void MeshesOptimizer::optimize(const QString &filePath) // Optimize the selected mesh
{
    NifFile nif;
    if(nif.Load(filePath.toStdString()) != 0)
    {
        QLogger::QLog_Error("MeshesOptimizer", tr("Cannot load mesh: ") + filePath);
        return;
    }

    OptOptions options;
    options.targetVersion.SetFile(CAO_MESHES_FILE_VERSION);
    options.targetVersion.SetStream(CAO_MESHES_STREAM);
    options.targetVersion.SetUser(CAO_MESHES_USER);

    ScanResult scanResult = scan(filePath);
    QString relativeFilePath = filePath.mid(filePath.indexOf("/meshes/", Qt::CaseInsensitive) + 1);

    //Headparts have to get a special optimization
    if(iMeshesOptimizationLevel >= 1 && bMeshesHeadparts && headparts.contains(relativeFilePath, Qt::CaseInsensitive))
    {
        options.bsTriShape = true;
        options.headParts = true;
        QLogger::QLog_Note("MeshesOptimizer", tr("Running NifOpt...")  + tr("Processing: ") + filePath + tr(" as an headpart due to necessary optimization"));
        nif.OptimizeFor(options);
    }
    else
    {
        switch (scanResult)
        {
        case doNotProcess: return;
        case good:
        case lightIssue:
            if(iMeshesOptimizationLevel >= 3)
            {
                options.bsTriShape = true;
                QLogger::QLog_Note("MeshesOptimizer", tr("Running NifOpt...")  + tr("Processing: ") + filePath + tr(" due to full optimization"));
                nif.OptimizeFor(options);
            }
            else if(iMeshesOptimizationLevel >= 2)
            {
                QLogger::QLog_Note("MeshesOptimizer", tr("Running NifOpt...")  + tr("Processing: ") + filePath + tr(" due to medium optimization"));
                nif.OptimizeFor(options);
            }
            break;
        case criticalIssue:
            options.bsTriShape = true;
            QLogger::QLog_Note("MeshesOptimizer", tr("Running NifOpt...")  + tr("Processing: ") + filePath + tr(" due to necessary optimization"));
            nif.OptimizeFor(options);
            break;
        }
    }
    QLogger::QLog_Trace("MeshesOptimizer", "Resaving mesh: " + filePath);
    nif.Save(filePath.toStdString());
}


void MeshesOptimizer::dryOptimize(const QString &filePath)
{
    ScanResult scanResult = scan(filePath);
    QString relativeFilePath = filePath.mid(filePath.indexOf("/meshes/", Qt::CaseInsensitive) + 1);

    //Headparts have to get a special optimization
    if(iMeshesOptimizationLevel >= 1 && bMeshesHeadparts && headparts.contains(relativeFilePath, Qt::CaseInsensitive))
        QLogger::QLog_Note("MeshesOptimizer", tr("Running NifOpt...")  + tr("Processing: ") + filePath + tr(" as an headpart due to necessary optimization"));
    else
    {
        switch (scanResult)
        {
        case doNotProcess: return;
        case good:
        case lightIssue:
            if(iMeshesOptimizationLevel >= 3)
                QLogger::QLog_Note("MeshesOptimizer", filePath + tr(" would be optimized due to full optimization"));

            else if(iMeshesOptimizationLevel >= 2)
                QLogger::QLog_Note("MeshesOptimizer", filePath + tr(" would be optimized due to medium optimization"));
            break;
        case criticalIssue:
            QLogger::QLog_Note("MeshesOptimizer", filePath + tr(" would be optimized due to necessary optimization"));
            break;
        }
    }
}





