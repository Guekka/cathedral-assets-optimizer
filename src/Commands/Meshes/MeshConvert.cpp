/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <QDirIterator>

#include "MeshConvert.hpp"
#include "Commands/Plugins/PluginsOperations.hpp"
#include "File/Meshes/MeshFile.hpp"
#include "Settings/Games.hpp"
#include "Utils/Algorithms.hpp"
#include "Utils/Filesystem.hpp"
#include "Utils/TypeConvert.hpp"
#include "Utils/wildcards.hpp"

namespace CAO {

MeshConvert::MeshConvert()
{
    Profiles::callWhenRunStart([this] {
        this->listHeadparts(currentProfile().getGeneralSettings(), currentProfile().getFileTypes());
    });
}

CommandResult MeshConvert::process(File &file) const
{
    auto nif = dynamic_cast<MeshResource *>(&file.getFile(true));
    if (!nif)
        return CommandResultFactory::getCannotCastFileResult();

    auto &sets          = currentProfile().getGeneralSettings();
    auto &game          = GameSettings::get(sets.eGame());
    NiVersion targetVer = nif->GetHeader().GetVersion();
    NiVersion gameVer   = game.cMeshesVersion().value();
    targetVer.SetFile(gameVer.File());
    targetVer.SetStream(gameVer.Stream());
    targetVer.SetUser(gameVer.User());

    OptOptions optOptions{.targetVersion  = targetVer,
                          .headParts      = isHeadpart(file.getInputFilePath()),
                          .removeParallax = false,
                          .mandatoryOnly  = false};

    nif->OptimizeFor(optOptions);

    return CommandResultFactory::getSuccessfulResult();
}

bool MeshConvert::isApplicable(File &file) const
{
    auto &patternSettings = file.patternSettings();
    int optLevel          = patternSettings.iMeshesOptimizationLevel();
    if (optLevel == 0)
        return false;

    const auto &game = GameSettings::get(currentProfile().getGeneralSettings().eGame());
    if (!game.cMeshesVersion().has_value())
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
