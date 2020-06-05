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

    if (!QDir(sUserPath()).exists() || sUserPath().size() < 5)
        return QString("This path does not exist or is shorter than 5 characters. Path: '%1'").arg(sUserPath());

    return std::nullopt;
}
} // namespace CAO
