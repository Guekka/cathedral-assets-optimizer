/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "MeshConvert.hpp"

namespace CAO {
QStringList MeshConvert::headpartList;

CommandResult MeshConvert::process(File &file, const OptionsCAO &options)
{
    auto meshFile = dynamic_cast<MeshFile *>(&file);
    if (!meshFile)
        return _resultFactory.getCannotCastFileResult();

    NifFile nifFile = meshFile->getFile();
    NiVersion niVersion;
    niVersion.SetUser(Profiles::meshesUser());
    niVersion.SetFile(Profiles::meshesFileVersion());
    niVersion.SetStream(Profiles::meshesStream());

    OptOptions optOptions;
    optOptions.targetVersion = niVersion;
    optOptions.headParts = isHeadpart(file.getName());

    nifFile.OptimizeFor(optOptions);
    meshFile->setFile(nifFile);

    return _resultFactory.getSuccessfulResult();
}

bool MeshConvert::isApplicable(File &file, const OptionsCAO &options)
{
    if (options.iMeshesOptimizationLevel == 0)
        return false;

    auto meshFile = dynamic_cast<MeshFile *>(&file);
    if (!meshFile)
        return false;

    //Listing headparts
    std::call_once(_onceHeadpartsFlag, [this, &options] { this->listHeadparts(options); });

    NifFile nif = meshFile->getFile();

    const bool headpart = isHeadpart(file.getName());

    for (const auto &shape : nif.GetShapes())
    {
        //Those cannot be converted
        if (shape->HasType<NiParticles>() || shape->HasType<NiParticleSystem>() || shape->HasType<NiParticlesData>())
            return false;

        // Check if shape has strips in the geometry or skin partition
        if (!shape->HasType<NiTriStrips>())
        {
            auto skinInst = nif.GetHeader().GetBlock<NiSkinInstance>(shape->GetSkinInstanceRef());
            if (skinInst)
            {
                auto skinPart = nif.GetHeader().GetBlock<NiSkinPartition>(skinInst->GetSkinPartitionRef());
                if (skinPart)
                    for (auto &partition : skinPart->partitions)
                        if (partition.numStrips > 0)
                            return true; //Critical issue
            }
        }
    }
    return (headpart && options.bMeshesHeadparts) || options.bMeshesResave || options.iMeshesOptimizationLevel >= 2;
}

bool MeshConvert::isHeadpart(const QString &filepath)
{
    const QString relativeFilePath = QDir::cleanPath(
        filepath.mid(filepath.indexOf("/meshes/", Qt::CaseInsensitive) + 1));
    return headpartList.contains(relativeFilePath, Qt::CaseInsensitive);
}

void MeshConvert::listHeadparts(const OptionsCAO &options)
{
    QFile &&customHeadpartsFile = Profiles::getFile("customHeadparts.txt");
    headpartList = FilesystemOperations::readFile(customHeadpartsFile,
                                                  [](QString &string) { return QDir::cleanPath(string); });

    if (headpartList.isEmpty())
    {
        PLOG_ERROR << "customHeadparts.txt not found. This can cause issue when optimizing meshes, as some headparts "
                      "won't be detected.";
    }

    const bool severalMods = options.mode == OptionsCAO::SeveralMods;
    const auto flags = severalMods ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;

    QDirIterator it(options.userPath, flags);
    for (const auto &plugin : FilesystemOperations::listPlugins(it))
        headpartList += PluginsOperations::listHeadparts(plugin);

    headpartList.removeDuplicates();
}

} // namespace CAO
