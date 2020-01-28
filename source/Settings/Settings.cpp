/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Settings.hpp"
#include "Settings/Profiles.hpp"

namespace CAO {

std::optional<QString> PatternSettings::isValid()
{
    if (iMeshesOptimizationLevel() > 3)
        return ("This meshes optimization level does not exist. Level: "
                + QString::number(iMeshesOptimizationLevel()));

    if (iTexturesTargetWidth() % 2 != 0 || iTexturesTargetHeight() % 2 != 0)
        return ("Textures target size has to be a power of two");

    return std::nullopt;
}

GeneralSettings::GeneralSettings(nlohmann::json j)
    : Settings(j)
{
}

QString GeneralSettings::isValid() const
{
    if (!QDir(sUserPath()).exists() || sUserPath().size() < 5) {
        return QString("This path does not exist or is shorter than 5 characters. Path: '%1'")
            .arg(sUserPath());
    }

    return QString();
}
} // namespace CAO
