/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
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

GeneralSettings::GeneralSettings(GeneralSettings &&other)
{
    json_ = std::move(other.json_);
}

GeneralSettings &GeneralSettings::operator=(const GeneralSettings &other)
{
    if (this != &other)
        json_ = other.json_;

    return *this;
}

GeneralSettings &GeneralSettings::operator=(GeneralSettings &&other)
{
    if (this != &other)
        json_ = std::move(other.json_);

    return *this;
}

std::optional<QString> GeneralSettings::isValid() const
{
    if (iBSAMaxSize() < 0 || iBSATexturesMaxSize() < 0)
        return QString("BSA Max size cannot be negative");

    if (sUserPath().size() < 5)
        return QString("This path is shorter than 5 characters.").arg(sUserPath());

    const QDir userDir(sUserPath());

    if (!userDir.exists() || sUserPath().size() < 5)
        return QString("This path does not exist. Path: '%1'").arg(sUserPath());

    const ModManager managedBy = findManager(userDir);

    if (eMode() == SeveralMods && managedBy == ModManager::None)
    {
        return QString("Several mods mode is enabled, but this path does not seem to be handled by a mod "
                       "manager.\n If you are sure you want to process this folder, please create a file "
                       "named '%1' in this folder. This is a necessary evil to ensure safety for your mods.\n"
                       "Path: '%2'")
            .arg(forceProcessFolder, sUserPath());
    }

    if (eMode() == SingleMod && managedBy != ModManager::None)
    {
        return QString("Several mods mode is enabled, but this path seems to be handled by a mod manager. "
                       "Path: '%1'")
            .arg(sUserPath());
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
    auto first10Dirs = QDir(dir).entryList(QDir::NoDotAndDotDot | QDir::Dirs) | rx::take(10);

    for (const QString &dirName : first10Dirs)
        if (QDir(dirName).exists("meta.ini"))
            return ModManager::MO2;

    //Kortex not yet handled

    return ModManager::None;
}
} // namespace CAO
