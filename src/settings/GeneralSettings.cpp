/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "GeneralSettings.hpp"

#include <flow.hpp>

#include <QDir>

namespace cao {
/*
std::optional<QString> GeneralSettings::isValid() const
{
    if (bsa_max_size() < 0.5 || iBSATexturesMaxSize() < 0.5)
        return tr("BSA Max size cannot be smaller than 0.5Gb");

    if (sInputPath().size() < 5)
        return tr("This path is shorter than 5 characters: Path: '%1'").arg(sInputPath());

    const QDir inputDir(sInputPath());

    if (!inputDir.exists() || sInputPath().size() < 5)
        return tr("Input path does not exist. Path: '%1'").arg(sInputPath());

    const ModManager managedBy = findManager(inputDir);

    if (eMode() == SeveralMods && managedBy == ModManager::None)
    {
        return tr("'Several mods' mode is enabled, but this path does not seem to be handled by a mod "
                  "manager.\n If you are sure you want to process this folder, please create a file "
                  "named '%1' in this folder. This is a necessary evil to ensure safety for your mods.\n"
                  "Path: '%2'")
            .arg(forceProcessFolder, sInputPath());
    }

    const bool isSingleModReady = managedBy == ModManager::None || managedBy == ModManager::ManualForced;
    if (eMode() == SingleMod && !isSingleModReady)
    {
        return tr("'Single mod' mode is enabled, but this path seems to be handled by a mod manager. "
                  "Path: '%1'")
            .arg(sInputPath());
    }

    if (bEnableOutputPath())
    {
        const QDir outputDir(sOutputPath());

        if (!outputDir.exists() || sOutputPath().size() < 5)
            return tr("Output path does not exist. Path: '%1'").arg(sOutputPath());
    }

    return std::nullopt;
}

GeneralSettings::ModManager GeneralSettings::findManager(const QDir &dir) const
{
    // FIXME
    return ModManager::None;
}
*/
} // namespace cao
