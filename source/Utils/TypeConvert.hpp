/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"

namespace CAO {
inline std::vector<QRegularExpression> toRegexVector(const QStringList &regexStrings,
                                                     bool useWildcard = false)
{
    std::vector<QRegularExpression> regexes;
    std::transform(regexStrings.begin(),
                   regexStrings.end(),
                   std::back_inserter(regexes),
                   [useWildcard](const QString &str) {
                       if (useWildcard)
                           return QRegularExpression(
                               QRegularExpression::wildcardToRegularExpression(str));
                       else
                           return QRegularExpression(str);
                   });
    return regexes;
}

inline QStringList toStringList(const std::vector<QRegularExpression> &regexes)
{
    QStringList regexStrings;
    std::transform(regexes.cbegin(),
                   regexes.cend(),
                   std::back_inserter(regexStrings),
                   [](const QRegularExpression &regex) { return regex.pattern(); });
    return regexStrings;
}

inline QStringList toStringList(const std::vector<std::string> &strings)
{
    QStringList stringList;
    std::transform(strings.cbegin(),
                   strings.cend(),
                   std::back_inserter(stringList),
                   [](const std::string &str) { return QString::fromStdString(str); });
    return stringList;
}

inline std::vector<std::string> toStringVector(const QStringList &strings)
{
    std::vector<std::string> stringVec(strings.size());
    std::transform(strings.begin(),
                   strings.end(),
                   std::back_inserter(stringVec),
                   [](const QString &str) { return str.toStdString(); });
    return stringVec;
}

} // namespace CAO
