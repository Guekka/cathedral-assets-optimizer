/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"

namespace CAO {
namespace JSON {

void readFromFile(nlohmann::json &json, const QString &filepath);
void saveToFile(const nlohmann::json &json, const QString &filepath);
nlohmann::json &splitKey(nlohmann::json &json, const QString &key);
const nlohmann::json &splitKey(const nlohmann::json &json, const QString &key);

template<typename T>
inline T getValueSafe(const nlohmann::json &j)
{
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
inline T getValue(const nlohmann::json &json, const QString &key)
{
    return getValueSafe<T>(splitKey(json, key));
}

template<>
inline QString getValue(const nlohmann::json &json, const QString &key)
{
    return QString::fromStdString(getValue<std::string>(json, key));
}

template<>
inline QStringList getValue(const nlohmann::json &json, const QString &key)
{
    QStringList list;
    for (const auto &str : getValue<std::vector<std::string>>(json, key))
        list << QString::fromStdString(str);
    return list;
}

template<class T>
inline void setValue(nlohmann::json &json, const QString &key, const T &value)
{
    splitKey(json, key) = value;
}

template<>
inline void setValue(nlohmann::json &json, const QString &key, const QString &value)
{
    setValue(json, key, value.toStdString());
}

template<>
inline void setValue(nlohmann::json &json, const QString &key, const QStringList &value)
{
    auto qstringVec = value.toVector().toStdVector();
    std::vector<std::string> stringVec(qstringVec.size());
    std::transform(qstringVec.begin(),
                   qstringVec.end(),
                   std::back_inserter(stringVec),
                   [](const QString &str) { return str.toStdString(); });
    setValue(json, key, stringVec);
}

}; // namespace JSON
} // namespace CAO
