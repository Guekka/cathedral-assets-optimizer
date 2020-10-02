/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <QDir>

#include "GeneralSettings.hpp"

namespace CAO {
GeneralSettings::GeneralSettings(nlohmann::json j)
{
    setJSON(std::move(j));
}

GeneralSettings::GeneralSettings(const GeneralSettings &other)
{
    json_ = other.json_;
}

GeneralSettings::GeneralSettings(GeneralSettings &&other) noexcept
{
    json_ = std::move(other.json_);
}

GeneralSettings &GeneralSettings::operator=(const GeneralSettings &other)
{
    if (this != &other)
        json_ = other.json_;

    return *this;
}

GeneralSettings &GeneralSettings::operator=(GeneralSettings &&other) noexcept
{
    if (this != &other)
        json_ = std::move(other.json_);

    return *this;
}

std::optional<QString> GeneralSettings::isValid() const
{
    //TODO
    if (iBSAMaxSize() < 0.5 || iBSATexturesMaxSize() < 0.5)
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
    /* Manual forced */
    if (dir.exists(forceProcessFolder))
        return ModManager::ManualForced;

    /* Vortex */
    if (dir.exists("__vortex_staging_folder"))
        return ModManager::Vortex;

    /* MO2 */
    //Checking 10 dirs should be enough. One of them should be enough actually, but...better be safe
    auto first10Dirs = QDir(dir).entryList(QDir::NoDotAndDotDot | QDir::Dirs) | rx::take(10)
                       | rx::to_vector();

    for (const QString &dirName : first10Dirs)
        if (QDir(dirName).exists("meta.ini"))
            return ModManager::MO2;

    //Kortex not yet handled

    return ModManager::None;
}
} // namespace CAO
