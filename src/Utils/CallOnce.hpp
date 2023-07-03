/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <functional>

class CallOnce
{
public:
    CallOnce() = default;

    CallOnce(const CallOnce &) = delete;
    CallOnce(CallOnce &&)      = default;

    CallOnce &operator=(const CallOnce &) = delete;
    CallOnce &operator=(CallOnce &&)      = delete;

    ~CallOnce() = default;

    bool call(const std::function<void()> &func);
    void reset();

private:
    bool called_ = false;
};
