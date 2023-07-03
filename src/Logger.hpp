/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

/* Custom functions for plog */

#pragma once

#include <plog/Log.h>

#include <QString>

namespace CAO {
void initCustomLogger(const QString &logPath);
} // namespace CAO

namespace plog {
class CustomFormatter
{
public:
    static util::nstring header();
    static util::nstring format(const Record &record);
};
} // namespace plog
