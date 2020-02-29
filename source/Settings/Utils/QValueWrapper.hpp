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
class QValueWrapper : public detail::QValueWrapperHelper
{
public:
    using Type = T;

    QValueWrapper() = default;

    QValueWrapper(const Type &val)
        : value_(val){};

    QValueWrapper(const QValueWrapper &other) { value_ = other.value_; }
    QValueWrapper(QValueWrapper &&other) { setValue(std::move(other.value_)); }

    const Type &operator()() const { return value(); }
    const Type &value() const { return value_; }
    virtual void setValue(const Type &newValue)
    {
        if (value_ != newValue)
            emit valueChanged();
        value_ = newValue;
    }

    void operator=(const QValueWrapper &other) { setValue(other.value_); }
    void operator=(const Type &value) { setValue(value); }

private:
    Type value_;
};

template<typename T>
class QJSONValueWrapper : public QValueWrapper<T>
{
public:
    using Type = T;
    using parentType = QValueWrapper<T>;

    //NOTE The JSON has to outlive the QJSONValueWrapper
    QJSONValueWrapper(std::unique_ptr<nlohmann::json> &j, std::string key)
        : json_(j)
        , key_(std::move(key))
    {
        const auto &json = *json_;
        assert(key.size() > 0);
        assert(json.is_null() || json.is_object());
        setValue(JSON::getValue<Type>(json, key_));
    }

    QJSONValueWrapper(const QJSONValueWrapper &other) = delete;
    QJSONValueWrapper(QJSONValueWrapper &&other) = delete;
    void operator=(const QJSONValueWrapper &other) = delete;

    virtual void setValue(const Type &newValue) override
    {
        parentType::setValue(newValue);
        JSON::setValue(*json_, key_, value());
    }

    void operator=(const Type &value) { setValue(value); }

private:
    std::unique_ptr<nlohmann::json> &json_;
    std::string key_;
};

} // namespace CAO
