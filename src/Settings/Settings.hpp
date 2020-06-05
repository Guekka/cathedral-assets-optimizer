
/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "GUI/MainWindow.hpp"
#include "Utils/QJSONValueWrapper.hpp"
#include "pch.hpp"

namespace CAO {
class Settings
{
public:
    //! \brief Checks if the current settings are allowed
    virtual std::optional<QString> isValid() const = 0;

    Settings() = default;
    Settings(const nlohmann::json &j);
    Settings(const Settings &other);
    Settings(Settings &&other);
    virtual ~Settings() = default;

    void operator=(const Settings &other);
    void operator=(Settings &&other);

    virtual nlohmann::json getJSON() const { return json_; }
    virtual void setJSON(const nlohmann::json &j) { json_ = j; }

protected:
    mutable nlohmann::json json_;
};
} // namespace CAO
