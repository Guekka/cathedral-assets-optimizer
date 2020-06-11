/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Utils/Algorithms.hpp"
#include "utils.hpp"

TEST_CASE("String find")
{
    std::string_view strTest = "abcdefghijklmnopqrstuvwxyz";
    CHECK_EQ(strFind(strTest, std::string_view("def")), 3);

    CHECK_EQ(strFind(strTest, std::string_view("DEF")), std::string::npos);

    CHECK_EQ(strFind(strTest, std::string_view("DEF"), false), 3);
}

TEST_CASE("String length")
{
    std::string strTest = "abcdefghijklmnopqrstuvwxyz";
    CHECK_EQ(strLength(strTest), 26);

    std::wstring wStrTest = L"abcdefghijklmnopqrstuvwxyz";
    CHECK_EQ(strLength(wStrTest), 26);

    const char *pCharTest = "abcdefghijklmnopqrstuvwxyz";
    CHECK_EQ(strLength(pCharTest), 26);

    const wchar_t *pWCharTest = L"abcdefghijklmnopqrstuvwxyz";
    CHECK_EQ(strLength(pWCharTest), 26);
}

TEST_CASE("String replace")
{
    std::string strTest = "abc123abc123abc123";
    replaceAll(strTest, std::string_view("abc"), std::string_view("123"));
    CHECK_EQ(strTest, std::string("123123123123123123"));

    std::wstring wStrTest = L"abc123abc123abc123";
    replaceAll(wStrTest, std::wstring_view(L"abc"), std::wstring_view(L"123"));
    CHECK_EQ(wStrTest, std::wstring(L"123123123123123123"));
}

TEST_CASE("String ends with")
{
    std::string_view value = "abcdefghijklmnopqrstuvwxyz";

    SUBCASE("Case sensitive")
    {
        CHECK(endsWith(value, std::string_view("xyz")));
        CHECK_FALSE(endsWith(value, std::string_view("XYZ")));
        CHECK_FALSE(endsWith(value, std::string_view("abc")));
    }
    SUBCASE("Case insensitive") { CHECK(endsWith(value, std::string_view("XYZ"), false)); }
}

TEST_CASE("String starts with")
{
    std::string_view value = "abcdefghijklmnopqrstuvwxyz";

    SUBCASE("Case sensitive")
    {
        CHECK(startsWith(value, std::string_view("abc")));
        CHECK_FALSE(startsWith(value, std::string_view("ABC")));
        CHECK_FALSE(startsWith(value, std::string_view("xyz")));
    }
    SUBCASE("Case insensitive") { CHECK(startsWith(value, std::string_view("ABC"), false)); }
}
