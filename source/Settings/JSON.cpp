/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "JSON.hpp"

namespace CAO {
JSON::JSON()
    : _json(nlohmann::json::value_t::object)
{}

JSON::JSON(const nlohmann::json &j)
    : _json(j)
{}

nlohmann::json &JSON::splitKey(const QString &key) const
{
    nlohmann::json *j = &_json;
    const auto &list = key.split("/");

    for (int i = 0; i < list.size(); ++i) {
        const auto &subStr = list[i].toStdString();
        j = &(j->operator[](subStr));
        if (j->is_null() && i != (list.size() - 1))
            *j = nlohmann::json::value_t::object;
    }
    return *j;
}

void JSON::saveToJSON(const QString &filepath) const
{
    std::fstream stream(std::filesystem::u8path(filepath.toStdString()), std::fstream::out);
    if (!stream.is_open())
        return;

    stream << _json.dump(4);
}

void JSON::readFromJSON(const QString &filepath)
{
    std::fstream stream(std::filesystem::u8path(filepath.toStdString()), std::fstream::in);
    if (!stream.is_open())
        return;

    stream >> _json;
}
} // namespace CAO
