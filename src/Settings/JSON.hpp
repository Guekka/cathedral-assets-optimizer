/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Utils/TypeConvert.hpp"
#include "pch.hpp"

//Custom serialization for Qt
namespace nlohmann {
template<>
struct adl_serializer<QString>
{
    static void to_json(json &j, const QString &str) { j = str.toStdString(); }
    static void from_json(const json &j, QString &str) { str = QString::fromStdString(j.get<std::string>()); }
};

template<>
struct adl_serializer<QStringList>
{
    static void to_json(json &j, const QStringList &strList) { j = CAO::toStringVector(strList); }
    static void from_json(const json &j, QStringList &strList)
    {
        strList = CAO::toStringList(j.get<std::vector<std::string>>());
    }
};
} // namespace nlohmann

namespace CAO {
namespace JSON {

using json_pointer = nlohmann::json_pointer<nlohmann::json>;

void readFromFile(nlohmann::json &json, const QString &filepath);
void saveToFile(const nlohmann::json &json, const QString &filepath);

void removeDuplicates(const nlohmann::json &master, std::vector<nlohmann::json> &jsons);

bool contains(const nlohmann::json &master, const nlohmann::json &subset);

json_pointer getPointer(const std::string &key);

template<typename T, typename ValueType = typename T::value_type>
bool contains(const nlohmann::json &json, const ValueType &val)
{
    static_assert(is_vector_v<T>, "Type must be a vector");
    auto ref = json.get_ref<T>();

    return std::any_of(ref.cbegin(), ref.cend(), [&val](const ValueType &el) { return el == val; });
}

template<typename T>
inline T getValue(const nlohmann::json &j)
{
    if (j.is_null())
        return T{};

    const nlohmann::json *json = &j;

    nlohmann::json defaultJ = T();
    const bool jIsInt = j.is_number_integer() || j.is_number_unsigned();
    const bool defaultJIsInt = defaultJ.is_number_integer() || defaultJ.is_number_unsigned();
    const bool areBothInt = jIsInt && defaultJIsInt;

    if (j.type() != defaultJ.type() && !areBothInt)
        json = &defaultJ;

    return json->get<T>();
}

template<class T>
inline T getValue(const nlohmann::json &json, const json_pointer &key)
{
    assert(json.is_object() || json.is_null());

    if (!json.contains(getPointer(key)))
        return getValue<T>(nlohmann::json{});

    return getValue<T>(json[key]);
}

template<class T>
inline T getValue(const nlohmann::json &json, const std::string &key)
{
    assert(key.size() > 0);
    assert(json.is_object() || json.is_null());

    if (!json.contains(getPointer(key)))
        return getValue<T>(nlohmann::json{});

    return getValue<T>(json[getPointer(key)]);
}

template<class T>
inline void setValue(nlohmann::json &json, const T &value)
{
    json = value;
}

template<class T>
inline void setValue(nlohmann::json &json, const std::string &key, const T &value)
{
    assert(json.is_object() || json.is_null());
    auto &&pointer = getPointer(key);
    setValue(json[pointer], value);
}

}; // namespace JSON
} // namespace CAO
