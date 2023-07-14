/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "JSON.hpp"

namespace cao::json {

void removeDuplicates(const nlohmann::json &master, std::vector<nlohmann::json> &jsons)
{
    /*
    auto flatMaster = master.flatten();
    jsons           = jsons | rx::transform([](auto &&j) { return j.flatten(); }) | rx::to_vector();

    for (auto &json : jsons)
        for (const auto &[key, value] : flatMaster.items())
            if (json.contains(key) && json[key] == value)
                json.erase(key);

    jsons = jsons | rx::filter([](auto &&j) { return !j.empty(); })
            | rx::transform([](auto &&j) { return j.unflatten(); }) | rx::to_vector();
            FIXME
            */
}

json_pointer getPointer(const std::string &key)
{
    assert(key.length() > 0);
    json_pointer j;
    if (key[0] == '/')
        j = json_pointer{key};
    else
        j = json_pointer{'/' + key};
    return j;
}

bool contains(const nlohmann::json &master, const nlohmann::json &subset)
{
    auto flat = subset.flatten();
    for (const auto &[key, val] : flat.items())
        if (!master.contains(key) || master[key] != val)
            return false;
    return true;
}
} // namespace cao::json
