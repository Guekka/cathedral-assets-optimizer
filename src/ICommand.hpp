/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "IFile.hpp"

class ICommand
{
public:
    virtual int performCommand(IFile &file) = 0;
    virtual int dryPerformCommand(IFile &file) = 0;
    virtual bool isBlacklisted(IFile &file) = 0;

    virtual ~ICommand();
};
