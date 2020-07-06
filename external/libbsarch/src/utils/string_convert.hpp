/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#ifdef QT
#include <QDebug>
#endif
#include <codecvt>
#include <locale>
#include <string>

namespace libbsarch {
#ifdef QT
std::wstring to_wstring(const QString &str);
std::string to_string(const QString &str);
QString to_qstring(const std::string &str);
QString to_qstring(const std::wstring &str);
#endif

std::string to_string(const std::wstring &str);
std::wstring to_wstring(const std::string &str);
} // namespace libbsarch
