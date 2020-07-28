/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MeshConvert.hpp"
#include "Utils/Algorithms.hpp"
#include "Utils/Filesystem.hpp"
#include "Utils/TypeConvert.hpp"
#include "Utils/wildcards.hpp"

namespace CAO {

MeshConvert::MeshConvert()
{
    getProfiles().callOncePerRun(callOnceHeadparts_, [this] {
        this->listHeadparts(currentProfile().getGeneralSettings(), currentProfile().getFileTypes());
    });
}

CommandResult MeshConvert::process(File &file) const
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
    optOptions.mandatoryOnly  = file.patternSettings().iMeshesOptimizationLevel() <= 2;
    optOptions.targetVersion  = niVersion;
    optOptions.removeParallax = false;
    optOptions.headParts      = isHeadpart(file.getInputFilePath());

    nif->OptimizeFor(optOptions);

    return _resultFactory.getSuccessfulResult();
}

bool MeshConvert::isApplicable(File &file) const
{
    auto& patternSettings = file.patternSettings();
    int optLevel          = patternSettings.iMeshesOptimizationLevel();
    if (optLevel == 0)
        return false;

    auto meshFile = dynamic_cast<const MeshResource *>(&file.getFile());
    if (!meshFile)
        return false;

    const bool headpart = isHeadpart(file.getInputFilePath()) && !patternSettings.bMeshesIgnoreHeadparts();
    const bool isSSECompatible = meshFile->IsSSECompatible();
    const bool resave          = optLevel >= 2;

    return !isSSECompatible || headpart || resave;
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

    QDirIterator it(settings.sInputPath(), flags);
    for (const auto &plugin : Filesystem::listPlugins(it))
    {
        const auto &result = PluginsOperations::listHeadparts(plugin);
        headparts.insert(headparts.end(), result.begin(), result.end());
    }

    remove_duplicates(headparts);
    filetypes.slMeshesHeadparts = headparts;
}

} // namespace CAO
