/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"

namespace CAO {
class QValueWrapperHelper : public QObject
{
    Q_OBJECT
public:
    QValueWrapperHelper(QObject *parent = nullptr)
        : QObject(parent)
    {}

signals:
    void valueChanged();
};

template<typename Type>
class QValueWrapper : public QValueWrapperHelper
{
public:
    using SubType = Type;

    QValueWrapper(QObject *parent = nullptr)
        : QValueWrapperHelper(parent){};
    QValueWrapper(const Type &val, QObject *parent = nullptr)
        : QValueWrapperHelper(parent)
        , value_(val){};

    QValueWrapper(const QValueWrapper &other) { value_ = other.value_; }
    QValueWrapper(QValueWrapper &&other) { value_ = std::move(other.value_); }

    const Type &value() const { return value_; }
    void setValue(const Type &newValue)
    {
        if (value_ != newValue)
            emit valueChanged(newValue);
        value_ = newValue;
    }

private:
    Type value_;
};

} // namespace CAO
