/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "CallOnce.hpp"

bool CallOnce::call(const std::function<void()> &func)
{
    if (called_)
        return false;

    func();
    called_ = true;
    return true;
}

void CallOnce::reset()
{
    called_ = false;
}
