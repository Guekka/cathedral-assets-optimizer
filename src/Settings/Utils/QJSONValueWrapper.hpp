/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings/JSON.hpp"
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
    QJSONValueWrapper(nlohmann::json &j, std::string key)
        : json_(j)
        , key_(std::move(key))
    {
        assert(key_.size() > 0);
        assert(j.is_null() || j.is_object());
    }

    QJSONValueWrapper(const QJSONValueWrapper &other) = delete;
    QJSONValueWrapper(QJSONValueWrapper &&other)      = delete;
    void operator=(const QJSONValueWrapper &other) = delete;

    void setValue(const Type &newValue)
    {
        if (!json_[JSON::getPointer(key_)].is_null() && value() == newValue)
            return;

        JSON::setValue(json_, key_, newValue);
        emit valueChanged();
    }

    Type value() const { return JSON::getValue<Type>(json_, key_); }
    Type value_or(const Type &fallback)
    {
        if (json_.contains(JSON::getPointer(key_)))
            return value();
        return fallback;
    }

    Type operator()() const { return value(); }
    void operator=(const Type &val) { setValue(val); }

private:
    nlohmann::json &json_;
    std::string key_;
};

} // namespace CAO
