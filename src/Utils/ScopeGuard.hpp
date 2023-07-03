/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <functional>
#include <utility>

class ScopeGuard
{
    using Function = std::function<void()>;
    std::vector<Function> functions_;

public:
    ScopeGuard(Function &&function)
        : functions_({function})
    {
    }
    ScopeGuard(const Function &function)
        : functions_({function})
    {
    }

    ~ScopeGuard()
    {
        while (!functions_.empty())
        {
            functions_.back()();
            functions_.pop_back();
        }
    }

    ScopeGuard(const ScopeGuard &) = delete;
    ScopeGuard(ScopeGuard &&other) noexcept
        : functions_(std::move(other.functions_))
    {
    }

    ScopeGuard &operator=(const ScopeGuard &) = delete;
    ScopeGuard &operator=(ScopeGuard &&other) noexcept
    {
        functions_ = std::move(other.functions_);
        return *this;
    }

    void add(const Function &func) { functions_.emplace_back(func); }
    void add(Function &&func) { functions_.emplace_back(std::move(func)); }

    void clear() { functions_.clear(); }

    ScopeGuard &operator+=(const Function &func)
    {
        add(func);
        return *this;
    }

    ScopeGuard &operator+=(Function &&func)
    {
        add(std::move(func));
        return *this;
    }
};
