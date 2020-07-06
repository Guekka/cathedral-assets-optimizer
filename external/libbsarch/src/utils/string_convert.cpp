/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "string_convert.hpp"

namespace libbsarch {

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
