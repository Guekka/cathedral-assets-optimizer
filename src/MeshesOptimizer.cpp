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

    QFile &&customHeadpartsFile = Profiles::getFile("customHeadparts.txt");

    headparts = FilesystemOperations::readFile(customHeadpartsFile);

    if (headparts.isEmpty())
    {
        PLOG_ERROR << "customHeadparts.txt not found. This can cause issue when optimizing meshes, as some headparts "
                      "won't be detected.";
    }
}

ScanResult MeshesOptimizer::scan(const QString &filePath) const
{
    NifFile nif;
    if (nif.Load(filePath.toStdString()) != 0)
    {
        PLOG_ERROR << "Cannot load mesh: " + filePath;
        return doNotProcess;
    }
    ScanResult result = good;

    NiVersion version;
    version.SetFile(Profiles::meshesFileVersion());
    version.SetStream(Profiles::meshesStream());
    version.SetUser(Profiles::meshesUser());

    if (version.IsSSE())
    {
        for (const auto &shape : nif.GetShapes())
        {
            if (shape->HasType<NiParticles>() || shape->HasType<NiParticleSystem>() || shape->HasType<NiParticlesData>())
                return doNotProcess;

            // Check if shape has strips in the geometry or skin partition
            bool hasStrips = shape->HasType<NiTriStrips>();
            if (!hasStrips)
            {
                auto skinInst = nif.GetHeader().GetBlock<NiSkinInstance>(shape->GetSkinInstanceRef());
                if (skinInst)
                {
                    auto skinPart = nif.GetHeader().GetBlock<NiSkinPartition>(skinInst->GetSkinPartitionRef());
                    if (skinPart)
                        for (auto &partition : skinPart->partitions)
                            if (partition.numStrips > 0)
                            {
                                hasStrips = true;
                                break;
                            }
                }
            }

            // Check to optimize all shapes or only mandatory ones
            if (hasStrips && result < criticalIssue)
                result = criticalIssue;
        }
    }
    else if (version.IsSK())
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
        if (it.fileName().contains(QRegularExpression("\\.es[plm]$")) && !it.fileInfo().isDir())
            headparts += PluginsOperations::listHeadparts(it.filePath());
    }

    headparts.removeDuplicates();
}

void MeshesOptimizer::optimize(const QString &filePath)
// Optimize the selected mesh
{
    NifFile nif;
    if (nif.Load(filePath.toStdString()) != 0)
    {
        PLOG_ERROR << "Cannot load mesh: " + filePath;
        return;
    }
    PLOG_VERBOSE << "Loading mesh: " + filePath;

    OptOptions options;
    options.targetVersion.SetFile(Profiles::meshesFileVersion());
    options.targetVersion.SetStream(Profiles::meshesStream());
    options.targetVersion.SetUser(Profiles::meshesUser());

    const ScanResult scanResult = scan(filePath);
    const QString relativeFilePath = filePath.mid(filePath.indexOf("/meshes/", Qt::CaseInsensitive) + 1);

    //Headparts have to get a special optimization
    if (iMeshesOptimizationLevel >= 1
        && (headparts.contains(relativeFilePath, Qt::CaseInsensitive) || relativeFilePath.contains("facegen")))
    {
        if (bMeshesHeadparts)
        {
            options.headParts = true;
            PLOG_INFO << "Optimizing: " + filePath + " as an headpart due to necessary optimization";
            nif.OptimizeFor(options);
        }
        else
            PLOG_VERBOSE << "Headpart mesh ignored: " + filePath;
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
                options.mandatoryOnly = false;
                PLOG_INFO << "Optimizing: " + filePath + " due to full optimization";
                nif.OptimizeFor(options);
            }
            else if (iMeshesOptimizationLevel >= 2)
            {
                options.mandatoryOnly = true;
                PLOG_INFO << "Optimizing: " + filePath + " due to medium optimization";
                nif.OptimizeFor(options);
            }
            break;
        case criticalIssue:
            if (iMeshesOptimizationLevel >= 1)
            {
                options.mandatoryOnly = false;
                PLOG_INFO << "Optimizing: " + filePath + " due to necessary optimization";
                nif.OptimizeFor(options);
            }
            break;
        }
    }
    if (bMeshesResave || (iMeshesOptimizationLevel >= 1 && scanResult >= criticalIssue) || iMeshesOptimizationLevel >= 2)
    {
        PLOG_VERBOSE << "Resaving mesh: " + filePath;
        nif.Save(filePath.toStdString());
    }
    PLOG_VERBOSE << "Closing mesh: " + filePath;
}

void MeshesOptimizer::dryOptimize(const QString &filePath) const
{
    const ScanResult scanResult = scan(filePath);
    const QString relativeFilePath = filePath.mid(filePath.indexOf("/meshes/", Qt::CaseInsensitive) + 1);

    //Headparts have to get a special optimization
    if (iMeshesOptimizationLevel >= 1 && bMeshesHeadparts && headparts.contains(relativeFilePath, Qt::CaseInsensitive))
        PLOG_INFO << filePath + " would be optimized as an headpart due to necessary optimization";
    else
    {
        switch (scanResult)
        {
        case doNotProcess: return;
        case good:
        case lightIssue:
            if (iMeshesOptimizationLevel >= 3)
                PLOG_INFO << filePath + " would be optimized due to full optimization";

            else if (iMeshesOptimizationLevel >= 2)
            {
                PLOG_INFO << filePath + " would be optimized due to medium optimization";
            }
            break;
        case criticalIssue: PLOG_INFO << filePath + " would be optimized due to necessary optimization"; break;
        }
    }
}
