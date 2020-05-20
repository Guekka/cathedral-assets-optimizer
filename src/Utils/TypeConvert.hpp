/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"

namespace CAO {

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

    auto toqs = [](auto &&e) { return toQString(e); };
    cont >>= pipes::transform(toqs) >>= pipes::push_back(stringList);
    return stringList;
} // namespace CAO

template<typename Container>
inline std::vector<std::string> toStringVector(Container &&cont)
{
    std::vector<std::string> stringVec(cont.size());
    cont >>= pipes::transform(toString) >>= pipes::push_back(stringVec);
    return stringVec;
}
} // namespace CAO
