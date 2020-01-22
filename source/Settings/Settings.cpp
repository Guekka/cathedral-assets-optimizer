/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Settings.hpp"
#include "Settings/Profiles.hpp"

namespace CAO {

Settings::Settings(nlohmann::json j)
    : _json(j)
{
}

QString Settings::isValid() const
{
    if (!QDir(getValue<QString>(sUserPath)).exists() || getValue<QString>(sUserPath).size() < 5)
    {
        return QString("This path does not exist or is shorter than 5 characters. Path: '%1'")
            .arg(getValue<QString>(sUserPath));
    }

    const auto &meshOptLevel = getValue<uint>(iMeshesOptimizationLevel);
    if (meshOptLevel > 3)
        return ("This meshes optimization level does not exist. Level: " + QString::number(meshOptLevel));

    if (getValue<uint>(iTexturesTargetWidth) % 2 != 0 || getValue<uint>(iTexturesTargetHeight) % 2 != 0)
        return ("Textures target size has to be a power of two");

    return QString();
}
} // namespace CAO
