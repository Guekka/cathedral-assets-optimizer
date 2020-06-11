/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Utils/TemplateMetaProgramming.hpp"
#include <algorithm>
#include <cassert>
#include <string>
#include <string_view>
#include <type_traits>

//Expects a range sorted in descending order
template<class It, class Predicate, class Sum>
[[nodiscard]] static It merge_if(It first, It last, Predicate &&predicate, Sum &&sum)
{
    if (first == last)
        return last;

    last--;
    while (first != last)
    {
        while (std::forward<Predicate>(predicate)(*first, *last))
        {
            *first = std::forward<Sum>(sum)(*first, *last);
            last--;
            if (first == last)
                return ++first;
        }
        first++;
    }
    return ++first;
}

template<class It, class Predicate>
[[nodiscard]] static It merge_if(It first, It last, Predicate &&predicate)
{
    using Type = std::remove_cv_t<std::remove_reference_t<decltype(*first)>>;
    auto plus  = [](const Type &first, const Type &second) { return first + second; };
    return merge_if(first, last, std::forward<Predicate>(predicate), plus);
}

template<class Container, class Predicate>
[[nodiscard]] static auto merge_if(Container &cont, Predicate &&predicate)
{
    using namespace std;
    return merge_if(begin(cont), end(cont), std::forward<Predicate>(predicate));
}

template<class Container, class Predicate, class Sum>
[[nodiscard]] static auto merge_if(Container &cont, Predicate &&pred, Sum &&sum)
{
    using namespace std;
    return merge_if(begin(cont), end(cont), std::forward<Predicate>(pred), std::forward<Sum>(sum));
}

template<class It, class ValueType>
[[nodiscard]] static auto contains(It begin, It end, const ValueType &value)
{
    return std::find(begin, end, value) != end;
}

template<class Container, class ValueType>
[[nodiscard]] static auto contains(const Container &cont, const ValueType &value)
{
    using namespace std;
    return contains(begin(cont), end(cont), value);
}

template<class Container, class Pred>
[[nodiscard]] static auto any_of(const Container &cont, Pred &&pred)
{
    using namespace std;
    return std::any_of(begin(cont), end(cont), std::forward<Pred>(pred));
}

template<class Container>
[[nodiscard]] static auto remove_duplicates(Container &cont)
{
    using namespace std;
    std::sort(begin(cont), end(cont));
    cont.erase(std::unique(begin(cont), end(cont)), end(cont));
}

template<class String>
constexpr size_t strLength(String &&str)
{
    if constexpr (CAO::is_equiv_v<String, const char *>)
        return strlen(std::forward<String>(str));

    else if constexpr (CAO::is_equiv_v<String, char>)
        return 1;

    else if constexpr (CAO::is_equiv_v<String, const wchar_t *>)
        return wcslen(std::forward<String>(str));

    else if constexpr (CAO::is_equiv_v<String, wchar_t>)
        return 1;

    else
        return (std::forward<String>(str)).length();
}

template<class CharT>
size_t strFind(std::basic_string_view<CharT> string,
               std::basic_string_view<CharT> snippet,
               bool caseSensitive = true,
               size_t fromPos     = 0)
{
    auto pred = [caseSensitive](char ch1, char ch2) {
        if (!caseSensitive)
        {
            ch1 = std::toupper(ch1);
            ch2 = std::toupper(ch2);
        }

        return ch1 == ch2;
    };

    using namespace std;

    if (cbegin(string) + fromPos > cend(string))
        return std::string::npos;

    auto it = search(cbegin(string) + fromPos, cend(string), cbegin(snippet), cend(snippet), pred);

    if (it != cend(string))
        return it - cbegin(string);
    else
        return std::string::npos; // not found
}

template<class CharT>
void replaceAll(std::basic_string<CharT> &source,
                std::basic_string_view<CharT> from,
                std::basic_string_view<CharT> to,
                bool caseSensitive = true)
{
    using String = std::basic_string<CharT>;
    using Size   = typename String::size_type;

    String newString;
    newString.reserve(source.length());

    Size lastPos = 0;

    Size findPos = strFind(std::basic_string_view<CharT>(source),
                           std::basic_string_view<CharT>(from),
                           caseSensitive);

    while (findPos != String::npos)
    {
        newString.append(source, lastPos, findPos - lastPos);
        newString += to;
        lastPos = findPos + strLength(from);

        findPos = strFind(std::basic_string_view<CharT>(source),
                          std::basic_string_view<CharT>(from),
                          caseSensitive,
                          lastPos);
    }

    // Care for the rest after last occurrence
    newString += source.substr(lastPos);

    source.swap(newString);
}

template<class CharT>
bool startsWith(std::basic_string_view<CharT> string,
                std::basic_string_view<CharT> prefix,
                bool caseSensitive = true)
{
    using namespace std;

    auto pred = [caseSensitive](char ch1, char ch2) {
        if (!caseSensitive)
        {
            ch1 = std::toupper(ch1);
            ch2 = std::toupper(ch2);
        }

        return ch1 == ch2;
    };

    return string.size() >= prefix.size() && std::equal(cbegin(prefix), cend(prefix), cbegin(string), pred);
}

template<class CharT>
bool endsWith(std::basic_string_view<CharT> string,
              std::basic_string_view<CharT> suffix,
              bool caseSensitive = true)
{
    using namespace std;

    auto pred = [caseSensitive](char ch1, char ch2) {
        if (!caseSensitive)
        {
            ch1 = std::toupper(ch1);
            ch2 = std::toupper(ch2);
        }

        return ch1 == ch2;
    };

    return string.size() >= suffix.size()
           && std::equal(crbegin(suffix), crend(suffix), crbegin(string), pred);
}
