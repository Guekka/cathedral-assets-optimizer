/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MeshConvert.hpp"
#include "Utils/Algorithms.hpp"
#include "Utils/TypeConvert.hpp"
#include "Utils/wildcards.hpp"

namespace CAO {
CommandResult MeshConvert::process(File &file)
{
    auto nif = dynamic_cast<MeshResource *>(&file.getFile(true));
    if (!nif)
        return _resultFactory.getCannotCastFileResult();

    NiVersion niVersion;
    auto &sets = currentProfile().getGeneralSettings();
    niVersion.SetUser(sets.iMeshesUser());
    niVersion.SetFile(sets.eMeshesFileVersion());
    niVersion.SetStream(sets.iMeshesStream());

    OptOptions optOptions;
    optOptions.targetVersion = niVersion;
    optOptions.headParts = isHeadpart(file.getName());

    nif->OptimizeFor(optOptions);

    return _resultFactory.getSuccessfulResult();
}

bool MeshConvert::isApplicable(File &file)
{
    auto& patternSettings = file.patternSettings();
    const int &optLevel = patternSettings.iMeshesOptimizationLevel();
    if (optLevel == 0)
        return false;

    auto meshFile = dynamic_cast<const MeshResource *>(&file.getFile());
    if (!meshFile)
        return false;

    //Listing headparts
    std::call_once(_onceHeadpartsFlag, [this] {
        this->listHeadparts(currentProfile().getGeneralSettings(), currentProfile().getFileTypes());
    });

    MeshResource nif = *meshFile;

    const bool headpart = isHeadpart(file.getName()) && patternSettings.bMeshesHeadparts();

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

    return headpart || patternSettings.bMeshesResave() || optLevel >= 2;
}

bool MeshConvert::isHeadpart(const QString &filepath)
{
    const auto &ft = currentProfile().getFileTypes();
    return ft.match(ft.slMeshesHeadparts(), filepath);
}

void MeshConvert::listHeadparts(const GeneralSettings &settings, FileTypes &filetypes)
{
    const bool severalMods = settings.eMode() == SeveralMods;
    const auto flags       = severalMods ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;

    std::vector<std::string> headparts = filetypes.slMeshesHeadparts();

    QDirIterator it(settings.sUserPath(), flags);
    for (const auto &plugin : Filesystem::listPlugins(it))
    {
        auto result       = PluginsOperations::listHeadparts(plugin);
        headparts.insert(headparts.end(), result.begin(), result.end());
    }

    remove_duplicates(headparts);
    filetypes.slMeshesHeadparts = headparts;
}

} // namespace CAO
