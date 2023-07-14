/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QString>
#include <QStringList>

namespace cao {

inline QString toQString(const std::string &str)
{
    return QString::fromStdString(str);
}

inline std::string toString(const QString &str)
{
    return str.toStdString();
}

template<typename Container>
inline QStringList toStringList(Container &&cont)
{
    QStringList stringList;
    stringList.reserve(cont.size());

    for (auto &&elem : std::forward<Container>(cont))
        stringList.push_back(toQString(std::forward<decltype(elem)>(elem)));

    return stringList;
}

template<typename Container>
inline std::vector<std::string> toStringVector(Container &&cont)
{
    std::vector<std::string> stringVec;
    stringVec.reserve(cont.size());

    for (auto &&elem : std::forward<Container>(cont))
        stringVec.emplace_back(toString(std::forward<decltype(elem)>(elem)));

    return stringVec;
}
} // namespace cao
