/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "JSON.hpp"

namespace CAO {
namespace JSON {

void saveToFile(const nlohmann::json &json, const QString &filepath)
{
    std::fstream stream(std::filesystem::u8path(filepath.toStdString()), std::fstream::out);
    if (!stream.is_open())
        return;

    stream << json.dump(4);
}

void readFromFile(nlohmann::json &json, const QString &filepath)
{
    std::fstream stream(std::filesystem::u8path(filepath.toStdString()), std::fstream::in);
    if (!stream.is_open())
        return;

    stream >> json;
}

nlohmann::json &splitKey(nlohmann::json &json, const QString &key)
{
    nlohmann::json *j = &json;
    const auto &list = key.split("/");

    for (int i = 0; i < list.size(); ++i) {
        const auto &subStr = list[i].toStdString();
        j = &(j->operator[](subStr));
        if (j->is_null() && i != (list.size() - 1))
            *j = nlohmann::detail::value_t::object;
    }
    return *j;
}

const nlohmann::json &splitKey(const nlohmann::json &json, const QString &key)
{
    const nlohmann::json *j = &json;
    const auto &list = key.split("/");

    for (int i = 0; i < list.size(); ++i) {
        const auto &subStr = list[i].toStdString();
        j = &(j->operator[](subStr));
    }
    return *j;
}

void removeDuplicates(nlohmann::json &master, std::vector<nlohmann::json> &jsons)
{
    master.flatten();
    for (const auto &[key, value] : master.items())
        for (auto &json : jsons) {
            json.flatten();
            if (value == json[key])
                json.erase(key);
        }

    master.unflatten();
    for (auto &j : jsons)
        j.unflatten();
}

} // namespace JSON
} // namespace CAO
