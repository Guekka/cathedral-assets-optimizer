/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

//See https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string

#include "string_convert.hpp"
#ifdef QT
#include <QString>
#endif

namespace libbsarch {
class convertible_string
{
public:
    // default ctor
    convertible_string() = default;

    /* conversion ctors */
    convertible_string(std::string value, bool to_native_path = true);
    convertible_string(const char *const val_array, bool to_native_path = true);
    convertible_string(const std::wstring &wvalue, bool to_native_path = true);
    convertible_string(const wchar_t *const wval_array, bool to_native_path = true);
#ifdef QT
    convertible_string(const QString &qsvalue, bool to_native_path = true);
#endif

    /* assignment operators */
    convertible_string &operator=(const std::string &value);
    convertible_string &operator=(const std::wstring &wvalue);
    convertible_string &operator=(const wchar_t *wvalue);
#ifdef QT
    convertible_string &operator=(const QString &qsvalue);
#endif

    /* implicit conversion operators */
    operator std::string() const;
    operator std::wstring() const;
    operator const wchar_t *() const;
#ifdef QT
    operator QString() const;
#endif

    /* Util */
    bool remove_substring(const convertible_string &sub_str);
    convertible_string &to_native_path();

private:
    std::string str_;
    bool auto_convert_to_native_path = true;
};
} // namespace libbsarch
