/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"

namespace CAO {
template<typename T>
using remove_cv_ref = std::remove_cv_t<std::remove_reference_t<T>>;

template<typename T, typename U>
using is_equiv = std::is_same<remove_cv_ref<T>, remove_cv_ref<U>>;

template<typename T, typename U>
constexpr bool is_equiv_v = is_equiv<T, U>::value;

template<class T>
struct is_vector : public std::false_type
{
    using type = T;
};

template<class T>
struct is_vector<std::vector<T>> : public std::true_type
{
    using type = T;
};

template<typename T>
constexpr bool is_vector_v = is_vector<T>::value;

template<typename T>
using is_vector_t = typename is_vector<T>::type;

} // namespace CAO
