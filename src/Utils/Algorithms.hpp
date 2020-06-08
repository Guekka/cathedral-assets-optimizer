/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <algorithm>
#include <cassert>
#include <type_traits>

//Expects a range sorted in descending order
template<class It, class Predicate, class Sum>
[[nodiscard]] static It merge_if(It first, It last, Predicate predicate, Sum sum)
{
    if (first == last)
        return last;

    last--;
    while (first != last)
    {
        while (predicate(*first, *last))
        {
            *first = sum(*first, *last);
            last--;
            if (first == last)
                return ++first;
        }
        first++;
    }
    return ++first;
}

template<class It, class Predicate>
[[nodiscard]] static It merge_if(It first, It last, Predicate predicate)
{
    using Type = std::remove_cv_t<std::remove_reference_t<decltype(*first)>>;
    auto plus  = [](const Type &first, const Type &second) { return first + second; };
    return merge_if(first, last, predicate, plus);
}

template<class Container, class Predicate>
[[nodiscard]] static auto merge_if(Container &&cont, Predicate predicate)
{
    return merge_if(cont.begin(), cont.end(), predicate);
}

template<class Container, class Predicate, class Sum>
[[nodiscard]] static auto merge_if(Container &&cont, Predicate pred, Sum sum)
{
    return merge_if(cont.begin(), cont.end(), pred, sum);
}

template<class It, class ValueType>
[[nodiscard]] static auto contains(It begin, It end, ValueType value)
{
    return std::find(begin, end, value) != end;
}

template<class Container, class ValueType>
[[nodiscard]] static auto contains(Container &&cont, ValueType value)
{
    return std::find(std::forward<Container>(cont).begin(), std::forward<Container>(cont).end(), value)
           != std::forward<Container>(cont).end();
}

template<class Container, class Pred>
[[nodiscard]] static auto any_of(Container &&cont, Pred &&pred)
{
    return std::any_of(std::forward<Container>(cont).begin(),
                       std::forward<Container>(cont).end(),
                       std::forward<Pred>(pred));
}

template<class Container>
[[nodiscard]] static auto remove_duplicates(Container &cont)
{
    std::sort(std::begin(cont), std::end(cont));
    cont.erase(std::unique(std::begin(cont), std::end(cont)), std::end(cont));
}
