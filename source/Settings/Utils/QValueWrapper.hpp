/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings/JSON.hpp"
#include "pch.hpp"

namespace CAO {
class MainWindow;
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
    QJSONValueWrapper(const QString &key, nlohmann::json &j)
        : parentType(JSON::getValue<Type>(j, key))
        , key_(key)
        , json_(&j)
    {}

    virtual void setValue(const Type &newValue) override
    {
        parentType::setValue(newValue);
        JSON::setValue(*json_, key_, value());
    }

    void operator=(const Type &value) { setValue(value); }

private:
    QString key_;
    nlohmann::json *json_;
};

} // namespace CAO
