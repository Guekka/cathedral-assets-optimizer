/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "GeneralSettings.hpp"

namespace CAO {
GeneralSettings::GeneralSettings(nlohmann::json j)
    : Settings(j)
{
}

GeneralSettings::GeneralSettings(const GeneralSettings &other)
    : Settings(other.json_)
{
}

GeneralSettings::GeneralSettings(GeneralSettings &&other)
    : Settings(std::move(other))
{
}

void GeneralSettings::operator=(const GeneralSettings &other)
{
    json_ = other.json_;
}

QString GeneralSettings::isValid() const
{
    if (!QDir(sUserPath()).exists() || sUserPath().size() < 5)
    {
        return QString("This path does not exist or is shorter than 5 characters. Path: '%1'").arg(sUserPath());
    }

    return QString();
}
} // namespace CAO
