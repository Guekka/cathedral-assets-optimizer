
/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"

namespace CAO {
class JSON final
{
public:
    JSON();
    JSON(const nlohmann::json &j);

    template<class T>
    T getValue(const QString &key) const
    {
        return getValueSafe<T>(splitKey(key));
    }

    template<>
    QString getValue(const QString &key) const
    {
        return QString::fromStdString(getValue<std::string>(key));
    }

    template<>
    QStringList getValue(const QString &key) const
    {
        QStringList list;
        for (const auto &str : getValue<std::vector<std::string>>(key))
            list << QString::fromStdString(str);
        return list;
    }

    template<class T>
    void setValue(const QString &key, const T &value)
    {        
        splitKey(key) = value;
    }

    template<>
    void setValue(const QString &key, const QString &value)
    {
        setValue(key, value.toStdString());
    }

    template<>
    void setValue(const QString &key, const QStringList &value)
    {
        auto qstringVec = value.toVector().toStdVector();
        std::vector<std::string> stringVec(qstringVec.size());
        std::transform(qstringVec.begin(),
                       qstringVec.end(),
                       std::back_inserter(stringVec),
                       [](const QString &str) { return str.toStdString(); });
        setValue(key, stringVec);
    }

    void readFromJSON(const QString &filepath);
    void saveToJSON(const QString &filepath) const;

    nlohmann::json &getInternalJSON() { return _json; }
    const nlohmann::json &getInternalJSON() const { return _json; }
    void setInternalJSON(nlohmann::json &j) { _json = j; }

private:
    mutable nlohmann::json
        _json; //We cannot provide a const getter without setting this member as mutable
    nlohmann::json &splitKey(const QString &key) const;

    template<typename T>
    T getValueSafe(const nlohmann::json &j) const
    {
        const nlohmann::json *json = &j;

        nlohmann::json defaultJ = T();
        bool areDifferentNumberTypes = (static_cast<int>(j.type()) + static_cast<int>(defaultJ.type())) == 11;
        //unsigned number = 6, signed number = 5
        //They can be converted between them, but their types aren't the same
        //If the sum is 11, it means that there is one uint and one int
        if (j.type() != defaultJ.type() && !areDifferentNumberTypes)
            json = &defaultJ;

        return json->get<T>();
    }
};
} // namespace CAO
