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

json_pointer getPointer(const QString &key)
{
    json_pointer j;
    if (key.startsWith('/'))
        j = json_pointer{key.toStdString()};
    else
        j = json_pointer{'/' + key.toStdString()};
    return j;
}

} // namespace JSON
} // namespace CAO
