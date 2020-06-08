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
