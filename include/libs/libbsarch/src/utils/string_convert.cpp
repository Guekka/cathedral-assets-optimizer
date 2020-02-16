/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "string_convert.hpp"

namespace libbsarch {
#ifdef QT
std::wstring to_wstring(const QString &str)
{
#ifdef _MSC_VER
    return std::wstring(reinterpret_cast<const wchar_t *>(str.utf16()));
#else
    return str.toStdWString();
#endif
}

std::string to_string(const QString &str)
{
    return str.toStdString();
}

QString to_qstring(const std::string &str)
{
    return QString::fromStdString(str);
}

QString to_qstring(const std::wstring &str)
{
#ifdef _MSC_VER
    return QString::fromUtf16(reinterpret_cast<const ushort *>(str.c_str()));
#else
    return QString::fromStdWString(str);
#endif
}
#endif

static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> s_converter;
std::string to_string(const std::wstring &str)
{
    return s_converter.to_bytes(str);
}

std::wstring to_wstring(const std::string &str)
{
    auto ws = s_converter.from_bytes(str);
    return ws;
}
} // namespace libbsarch
