/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <nlohmann/json.hpp>

#include <fstream>

namespace cao::json {

template<typename T>
[[nodiscard]] inline auto read_from_file(const std::filesystem::path &filepath) -> T
{
    std::fstream stream(filepath, std::fstream::in);
    if (!stream)
        return T{};

    nlohmann::json json;
    stream >> json;

    return json.get<T>();
}

[[nodiscard]] inline auto save_to_file(const nlohmann::json &json, const std::filesystem::path &filepath)
    -> bool
{
    std::fstream stream(filepath, std::fstream::out);
    if (!stream)
        return false;

    stream << json.dump(4);
    return true;
}

} // namespace cao::json
