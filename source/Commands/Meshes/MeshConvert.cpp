/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "MeshConvert.hpp"

namespace CAO {
QStringList MeshConvert::headpartList;

CommandResult MeshConvert::process(File &file, const Settings &settings)
{
    auto nif = dynamic_cast<MeshResource *>(&file.getFile(true));
    if (!nif)
        return _resultFactory.getCannotCastFileResult();

    NiVersion niVersion;
    niVersion.SetUser(settings.getMandatoryValue<int>(AdvancedSettings::iMeshesUser));
    niVersion.SetFile(settings.getMandatoryValue<NiFileVersion>(AdvancedSettings::eMeshesFileVersion));
    niVersion.SetStream(settings.getMandatoryValue<int>(AdvancedSettings::iMeshesStream));

    OptOptions optOptions;
    optOptions.targetVersion = niVersion;
    optOptions.headParts = isHeadpart(file.getName());

    nif->OptimizeFor(optOptions);

    return _resultFactory.getSuccessfulResult();
}

bool MeshConvert::isApplicable(File &file, const Settings &settings)
{
    const int &optLevel = settings.getMandatoryValue<int>(StandardSettings::iMeshesOptimizationLevel);
    if (optLevel == 0)
        return false;

    auto meshFile = dynamic_cast<const MeshResource *>(&file.getFile());
    if (!meshFile)
        return false;

    //Listing headparts
    std::call_once(_onceHeadpartsFlag, [this, &settings] { this->listHeadparts(settings); });

    MeshResource nif = *meshFile;

    const bool headpart = isHeadpart(file.getName())
                          && settings.getMandatoryValue<bool>(StandardSettings::bMeshesHeadparts);

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

    return headpart || settings.getMandatoryValue<bool>(StandardSettings::bMeshesResave) || optLevel >= 2;
}

bool MeshConvert::isHeadpart(const QString &filepath)
{
    const QString relativeFilePath = QDir::cleanPath(
        filepath.mid(filepath.indexOf("/meshes/", Qt::CaseInsensitive) + 1));
    return headpartList.contains(relativeFilePath, Qt::CaseInsensitive);
}

void MeshConvert::listHeadparts(const Settings &settings)
{
    QFile &&customHeadpartsFile = Profiles::getFile("customHeadparts.txt");
    headpartList = FilesystemOperations::readFile(customHeadpartsFile,
                                                  [](QString &string) { return QDir::cleanPath(string); });

    if (headpartList.isEmpty())
    {
        PLOG_ERROR << "customHeadparts.txt not found. This can cause issue when optimizing meshes, as some headparts "
                      "won't be detected.";
    }

    const bool severalMods = settings.getMandatoryValue<StandardSettings::OptimizationMode>(StandardSettings::eMode)
                             == StandardSettings::SeveralMods;
    const auto flags = severalMods ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;

    QDirIterator it(settings.getMandatoryValue<QString>(StandardSettings::sUserPath), flags);
    for (const auto &plugin : FilesystemOperations::listPlugins(it))
        headpartList += PluginsOperations::listHeadparts(plugin);

    headpartList.removeDuplicates();
}

} // namespace CAO
