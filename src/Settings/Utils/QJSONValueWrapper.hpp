/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings/JSON.hpp"
#include "Utils/TemplateMetaProgramming.hpp"
#include "pch.hpp"

namespace CAO {
namespace detail {
class QValueWrapperHelper : public QObject
{
    Q_OBJECT

signals:
    void valueChanged();
};
} // namespace detail

template<typename T>
class QJSONValueWrapper : public detail::QValueWrapperHelper
{
public:
    using Type = T;

    //NOTE The JSON has to outlive the QJSONValueWrapper
    QJSONValueWrapper(nlohmann::json &j, JSON::json_pointer key)
        : json_(j)
        , key_(std::move(key))
    {
        assert(j.is_null() || j.is_object());
    }

    ~QJSONValueWrapper() = default;

    QJSONValueWrapper(const QJSONValueWrapper &other) = delete;
    QJSONValueWrapper(QJSONValueWrapper &&other)      = delete;
    void operator=(const QJSONValueWrapper &other) = delete;
    void operator=(QJSONValueWrapper &&other) = delete;

    void setValue(const Type &newValue)
    {
        if (!json_[key_].is_null() && value() == newValue)
            return;

        JSON::setValue(json_, key_, newValue);
        emit valueChanged();
    }

    [[nodiscard]] Type value() const { return JSON::getValue<Type>(json_, key_); }
    [[nodiscard]] Type value_or(const Type &fallback)
    {
        if (json_.contains(key_))
            return value();
        return fallback;
    }

    [[nodiscard]] Type operator()() const { return value(); }
    QJSONValueWrapper &operator=(const Type &val)
    {
        setValue(val);
        return *this;
    }

    void insert(const Type &val, bool allowDups = true)
    {
        static_assert(is_vector_v<Type>, "Type must be a vector");

        nlohmann::json &j = json_[key_];

        if (allowDups)
            j.insert(j.end(), val.cbegin(), val.cend());

        else
            for (const auto &el : val)
                insert(el);
    }

    template<class ValueType = typename Type::value_type>
    void insert(const ValueType &val, bool allowDups = true)
    {
        static_assert(is_vector_v<Type>, "Type must be a vector");

        nlohmann::json &j = json_[key_];

        if (allowDups)
            j.push_back(val);

        else if (!JSON::contains(j, val))
            j.push_back(val);
    }

private:
    nlohmann::json &json_;
    JSON::json_pointer key_;
};

} // namespace CAO
