/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <functional>
#include <utility>

class exception_guard
{
    using function = std::function<void()>;
    std::vector<function> functions_;

public:
    exception_guard(function &&function)
        : functions_({function})
    {}
    exception_guard(const function &function)
        : functions_({function})
    {}

    exception_guard(const exception_guard &) = delete;
    exception_guard(exception_guard &&other) { functions_ = std::move(other.functions_); }

    ~exception_guard()
    {
        if (!std::uncaught_exceptions())
            return;

        while (!functions_.empty())
        {
            functions_.back()();
            functions_.pop_back();
        }
    }

    void add(const function &func) { functions_.emplace_back(func); }
    void add(function &&func) { functions_.emplace_back(std::move(func)); }

    void clear() { functions_.clear(); }

    exception_guard &operator+=(const function &func)
    {
        add(func);
        return *this;
    }

    exception_guard &operator+=(function &&func)
    {
        add(std::move(func));
        return *this;
    }
};
