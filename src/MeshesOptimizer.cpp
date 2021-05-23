/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MeshesOptimizer.h"
#include "FilesystemOperations.h"

using namespace nifly;

MeshesOptimizer::MeshesOptimizer(bool processHeadparts, int optimizationLevel, bool resaveMeshes)
    : bMeshesHeadparts(processHeadparts), bMeshesResave(resaveMeshes),
      iMeshesOptimizationLevel(optimizationLevel)
{}

ScanResult MeshesOptimizer::scan(NifFile &nif) const
{
    if (!nif.IsValid())
        return doNotProcess;

    NiVersion version;
    version.SetFile(Profiles::meshesFileVersion());
    version.SetStream(Profiles::meshesStream());
    version.SetUser(Profiles::meshesUser());

    if (!nif.IsSSECompatible() || version.IsSK())
        return criticalIssue;
    else
        return good;
}

void MeshesOptimizer::listHeadparts(const QString &directory)
{
    QFile &&customHeadpartsFile = Profiles::getFile("customHeadparts.txt");
    headparts = FilesystemOperations::readFile(customHeadpartsFile, [](QString &string) {
        return QDir::cleanPath(string);
    });

    if (headparts.isEmpty()) {
        PLOG_ERROR << "customHeadparts.txt not found. This can cause issue when optimizing meshes, "
                      "as some headparts "
                      "won't be detected.";
    }

    QDirIterator it(directory, QDirIterator::Subdirectories);
    for (const auto &plugin : FilesystemOperations::listPlugins(it))
        headparts += PluginsOperations::listHeadparts(plugin);

    headparts.removeDuplicates();
}

void MeshesOptimizer::optimize(const QString &filepath)
// Optimize the selected mesh
{
    auto [loadResult, nif] = loadMesh(filepath);
    if (!loadResult)
        return;

    OptOptions options;
    options.targetVersion.SetFile(Profiles::meshesFileVersion());
    options.targetVersion.SetStream(Profiles::meshesStream());
    options.targetVersion.SetUser(Profiles::meshesUser());
    options.removeParallax = false;

    const ScanResult scanResult = scan(nif);
    const QString relativeFilePath = filepath.mid(filepath.indexOf("/meshes/", Qt::CaseInsensitive)
                                                  + 1);

    bool processedHeadpart = false;
    //Headparts have to get a special optimization
    if (iMeshesOptimizationLevel >= 1
        && (headparts.contains(relativeFilePath, Qt::CaseInsensitive)
            || relativeFilePath.contains("facegen", Qt::CaseInsensitive))) {
        if (bMeshesHeadparts) {
            options.headParts = true;
            PLOG_INFO << "Optimizing: " + filepath
                             + " as an headpart due to necessary optimization";
            nif.OptimizeFor(options);
            processedHeadpart = true;
        } else
            PLOG_VERBOSE << "Headpart mesh ignored: " + filepath;
    } else {
        switch (scanResult) {
        case doNotProcess:
            return;
        case good:
        case lightIssue:
            if (iMeshesOptimizationLevel >= 3) {
                PLOG_INFO << "Optimizing: " + filepath + " due to full optimization";
                nif.OptimizeFor(options);
            }
            break;
        case criticalIssue:
            if (iMeshesOptimizationLevel >= 1) {
                PLOG_INFO << "Optimizing: " + filepath + " due to necessary optimization";
                nif.OptimizeFor(options);
            }
            break;
        }
    }

    const auto modifiedMesh = bMeshesResave
                              || (iMeshesOptimizationLevel >= 1 && scanResult >= criticalIssue)
                              || iMeshesOptimizationLevel >= 2;

    //Renaming textures referenced in mesh if TGA were converted to DDS
    const auto renamedReferencedTextures = Profiles::texturesConvertTga()
                                           && renameReferencedTexturesExtension(nif);
    PLOG_VERBOSE_IF(renamedReferencedTextures)
        << "Renamed referenced textures from TGA to DDS in " + filepath;

    if (modifiedMesh || renamedReferencedTextures || processedHeadpart)
        saveMesh(nif, filepath);
    PLOG_VERBOSE << "Closing mesh: " + filepath;
}

void MeshesOptimizer::dryOptimize(const QString &filepath) const
{
    auto [loadResult, nif] = loadMesh(filepath);
    if (!loadResult)
        return;

    const ScanResult scanResult = scan(nif);
    const QString relativeFilePath = filepath.mid(filepath.indexOf("/meshes/", Qt::CaseInsensitive)
                                                  + 1);

    //Headparts have to get a special optimization
    if (iMeshesOptimizationLevel >= 1 && bMeshesHeadparts
        && headparts.contains(relativeFilePath, Qt::CaseInsensitive))
        PLOG_INFO << filepath + " would be optimized as an headpart due to necessary optimization";
    else {
        switch (scanResult) {
        case doNotProcess:
            return;
        case good:
        case lightIssue:
            if (iMeshesOptimizationLevel >= 3)
                PLOG_INFO << filepath + " would be optimized due to full optimization";

            else if (iMeshesOptimizationLevel >= 2) {
                PLOG_INFO << filepath + " would be optimized due to medium optimization";
            }
            break;
        case criticalIssue:
            PLOG_INFO << filepath + " would be optimized due to necessary optimization";
            break;
        }
    }
}

bool MeshesOptimizer::renameReferencedTexturesExtension(NifFile &file)
{
    bool meshChanged = false;
    constexpr int limit = 10; // Only 10 slots

    for (auto shape : file.GetShapes()) {
        int texCounter = 0;
        for (int i = 0; i < limit; ++i) {
            std::string tex;
            file.GetTextureSlot(shape, tex, texCounter);
            if (tex.empty())
                continue;

            QString qsTex = QString::fromStdString(tex);
            if (qsTex.contains(".tga", Qt::CaseInsensitive)) {
                qsTex.replace(".tga", ".dds", Qt::CaseInsensitive);
                tex = qsTex.toStdString();
                file.SetTextureSlot(shape, tex, texCounter);
                tex = qsTex.replace(".tga", ".dds", Qt::CaseInsensitive).toStdString();
                meshChanged = true;
            }
            if (++texCounter > limit) {
                PLOG_ERROR << "Failed to renamed referenced textures from TGA to "
                              "DDS in this mesh";
                return false;
            }
        }
    }
    return meshChanged;
}

std::tuple<bool, NifFile> MeshesOptimizer::loadMesh(const QString &filepath) const
{
    PLOG_VERBOSE << "Loading mesh: " + filepath;

    NifLoadOptions loadOptions;
    loadOptions.isTerrain = (filepath.endsWith("btr", Qt::CaseInsensitive)
                             || filepath.endsWith("bto", Qt::CaseInsensitive));

    NifFile nif;
    if (nif.Load(filepath.toStdU16String(), loadOptions)) {
        PLOG_ERROR << "Cannot load mesh: " + filepath;
        return std::make_tuple(false, std::move(nif));
    }
    return std::make_tuple(true, std::move(nif));
}

bool MeshesOptimizer::saveMesh(NifFile &nif, const QString &filepath) const
{
    PLOG_VERBOSE << "Saving mesh: " + filepath;
    if (nif.Save(filepath.toStdU16String())) {
        PLOG_ERROR << "Cannot save mesh: " + filepath;
        return false;
    }
    return true;
}
