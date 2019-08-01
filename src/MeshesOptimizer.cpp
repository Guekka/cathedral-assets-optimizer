/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MeshesOptimizer.h"

MeshesOptimizer::MeshesOptimizer(bool processHeadparts, int optimizationLevel, bool resaveMeshes)
    : bMeshesHeadparts(processHeadparts)
    , bMeshesResave(resaveMeshes)
    , iMeshesOptimizationLevel(optimizationLevel)
{
    //Reading custom headparts file to add them to the list.
    //Done in the constructor since the file won't change at runtime.

    QFile customHeadpartsFile("resources/customHeadparts.txt");
    if (customHeadpartsFile.open(QIODevice::ReadOnly))
    {
        QTextStream ts(&customHeadpartsFile);
        while (!ts.atEnd())
        {
            QString readLine = ts.readLine();
            if (readLine.left(1) != "#" && !readLine.isEmpty())
                headparts << readLine;
        }
    }
    else
        PLOG_ERROR << tr("No custom headparts file found. Vanilla headparts won't be detected.");
}

ScanResult MeshesOptimizer::scan(const QString &filePath)
{
    NifFile nif;
    if (nif.Load(filePath.toStdString()) != 0)
    {
        PLOG_ERROR << tr("Cannot load mesh: ") + filePath;
        return doNotProcess;
    }
    ScanResult result = good;

    if (CAO_GET_CURRENT_GAME == Games::SSE)
    {
        for (const auto &shape : nif.GetShapes())
        {
            if (shape->HasType<NiTriStrips>() || shape->HasType<bhkMultiSphereShape>())
                if (result < criticalIssue)
                    result = criticalIssue;

            if (shape->HasType<NiParticles>() || shape->HasType<NiParticleSystem>() || shape->HasType<NiParticlesData>())
            {
                return doNotProcess;
            }
        }
    }
    else if (CAO_GET_CURRENT_GAME == Games::TES5)
        result = criticalIssue;
    else
        result = doNotProcess;

    return result;
}

void MeshesOptimizer::listHeadparts(const QString &directory)
{
    QDirIterator it(directory, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        it.next();
        if (it.fileName().contains(QRegularExpression("\\.es[plm]$")))
            headparts += PluginsOperations::listHeadparts(it.filePath());
    }

    headparts.removeDuplicates();
}

void MeshesOptimizer::optimize(const QString &filePath) // Optimize the selected mesh
{
    NifFile nif;
    if (nif.Load(filePath.toStdString()) != 0)
    {
        PLOG_ERROR << tr("Cannot load mesh: ") + filePath;
        return;
    }

    OptOptions options;
    options.targetVersion.SetFile(CAO_MESHES_FILE_VERSION);
    options.targetVersion.SetStream(CAO_MESHES_STREAM);
    options.targetVersion.SetUser(CAO_MESHES_USER);

    ScanResult scanResult = scan(filePath);
    QString relativeFilePath = filePath.mid(filePath.indexOf("/meshes/", Qt::CaseInsensitive) + 1);

    //Headparts have to get a special optimization
    if (iMeshesOptimizationLevel >= 1 && bMeshesHeadparts && headparts.contains(relativeFilePath, Qt::CaseInsensitive))
    {
        options.bsTriShape = true;
        options.headParts = true;
        PLOG_INFO << tr("Running NifOpt...") + tr("Processing: ") + filePath
                         + tr(" as an headpart due to necessary optimization");
        nif.OptimizeFor(options);
    }
    else
    {
        switch (scanResult)
        {
        case doNotProcess: return;
        case good:
        case lightIssue:
            if (iMeshesOptimizationLevel >= 3)
            {
                options.bsTriShape = true;
                PLOG_INFO << tr("Running NifOpt...") + tr("Processing: ") + filePath + tr(" due to full optimization");
                nif.OptimizeFor(options);
            }
            else if (iMeshesOptimizationLevel >= 2)
            {
                PLOG_INFO << tr("Running NifOpt...") + tr("Processing: ") + filePath
                                 + tr(" due to medium optimization");
                nif.OptimizeFor(options);
            }
            break;
        case criticalIssue:
            options.bsTriShape = true;
            PLOG_INFO << tr("Running NifOpt...") + tr("Processing: ") + filePath + tr(" due to necessary optimization");
            nif.OptimizeFor(options);
            break;
        }
    }
    if (bMeshesResave)
    {
        PLOG_VERBOSE << "Resaving mesh: " + filePath;
        nif.Save(filePath.toStdString());
    }
}

void MeshesOptimizer::dryOptimize(const QString &filePath)
{
    ScanResult scanResult = scan(filePath);
    QString relativeFilePath = filePath.mid(filePath.indexOf("/meshes/", Qt::CaseInsensitive) + 1);

    //Headparts have to get a special optimization
    if (iMeshesOptimizationLevel >= 1 && bMeshesHeadparts && headparts.contains(relativeFilePath, Qt::CaseInsensitive))
        PLOG_INFO << tr("Running NifOpt...") + tr("Processing: ") + filePath
                         + tr(" as an headpart due to necessary optimization");
    else
    {
        switch (scanResult)
        {
        case doNotProcess: return;
        case good:
        case lightIssue:
            if (iMeshesOptimizationLevel >= 3)
                PLOG_INFO << filePath + tr(" would be optimized due to full optimization");

            else if (iMeshesOptimizationLevel >= 2)
            {
                PLOG_INFO << filePath + tr(" would be optimized due to medium optimization");
            }
            break;
        case criticalIssue: PLOG_INFO << filePath + tr(" would be optimized due to necessary optimization"); break;
        }
    }
}
