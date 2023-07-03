/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "nlohmann/json.hpp"

#include <QString>
#include <optional>

namespace CAO {
class Settings
{
public:
    //! \brief Checks if the current settings are allowed
    virtual std::optional<QString> isValid() const = 0;

    Settings()                 = default;
    Settings(const Settings &) = delete;
    Settings(Settings &&)      = delete;
    virtual ~Settings()        = default;

    Settings &operator=(const Settings &) = delete;
    Settings &operator=(Settings &&)      = delete;

    virtual nlohmann::json getJSON() const { return json_; }
    virtual void setJSON(const nlohmann::json &j) { json_ = j; }

protected:
    nlohmann::json json_;
};
} // namespace CAO
