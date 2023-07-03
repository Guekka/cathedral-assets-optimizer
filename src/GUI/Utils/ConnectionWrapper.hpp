/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "../Utils.hpp"

#include <QMetaObject>
#include <QObject>
#include <vector>

namespace CAO {
namespace detail {
class RAIIConnection
{
    QMetaObject::Connection connection_;

public:
    RAIIConnection(QMetaObject::Connection &&connection)
        : connection_(connection)
    {
    }

    RAIIConnection(const RAIIConnection &) = delete;
    RAIIConnection(RAIIConnection &&other)
        : connection_(std::exchange(other.connection_, {}))
    {
    }

    RAIIConnection &operator=(const RAIIConnection &) = delete;
    RAIIConnection &operator=(RAIIConnection &&)      = delete;

    ~RAIIConnection() { QObject::disconnect(connection_); }
};
} // namespace detail

class ConnectionWrapper
{
public:
    void disconnectAll() { connections_.clear(); }

protected:
    template<typename... Args>
    void connect(Args &&...args)
    {
        connections_.emplace_back(QObject::connect(std::forward<Args>(args)...));
    }

    template<typename... Args>
    void connectWrapper(Args &&...args)
    {
        connections_.emplace_back(CAO::connectWrapper(std::forward<Args>(args)...));
    }

private:
    std::vector<detail::RAIIConnection> connections_;
};
} // namespace CAO
