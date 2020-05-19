/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Utils/TypeConvert.hpp"
#include "pch.hpp"

namespace CAO {
namespace JSON {

using json_pointer = nlohmann::json_pointer<nlohmann::json>;

void readFromFile(nlohmann::json &json, const QString &filepath);
void saveToFile(const nlohmann::json &json, const QString &filepath);

void removeDuplicates(const nlohmann::json &master, std::vector<nlohmann::json> &jsons);

json_pointer getPointer(const std::string &key);

template<typename T>
inline T getValue(nlohmann::json j)
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

template<>
inline QString getValue(nlohmann::json j)
{
    return QString::fromStdString(getValue<std::string>(j));
}

template<>
inline QStringList getValue(nlohmann::json j)
{
    return toStringList(getValue<std::vector<std::string>>(j));
}

template<class T>
inline T getValue(nlohmann::json json, const std::string &key)
{
    assert(key.size() > 0);
    assert(json.is_object() || json.is_null());
    return getValue<T>(json[getPointer(key)]);
}

template<>
inline QString getValue(nlohmann::json json, const std::string &key)
{
    return QString::fromStdString(getValue<std::string>(json, key));
}

template<>
inline QStringList getValue(nlohmann::json json, const std::string &key)
{
    return toStringList(getValue<std::vector<std::string>>(json, key));
}

template<class T>
inline void setValue(nlohmann::json &json, const T &value)
{
    json = value;
}

template<>
inline void setValue(nlohmann::json &json, const QString &value)
{
    setValue(json, value.toStdString());
}

template<>
inline void setValue(nlohmann::json &json, const QStringList &value)
{
    setValue(json, toStringVector(value));
}

template<class T>
inline void setValue(nlohmann::json &json, const std::string &key, const T &value)
{
    assert(json.is_object() || json.is_null());
    auto &&pointer = getPointer(key);
    setValue(json[pointer], value);
}

template<>
inline void setValue(nlohmann::json &json, const std::string &key, const QString &value)
{
    setValue(json, key, value.toStdString());
}

template<>
inline void setValue(nlohmann::json &json, const std::string &key, const QStringList &value)
{
    setValue(json, key, toStringVector(value));
}

}; // namespace JSON
} // namespace CAO
