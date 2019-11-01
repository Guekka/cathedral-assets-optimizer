/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "File.hpp"

namespace CAO {
int File::loadFromMemory(const void *pSource, const size_t &size, const QString &fileName)
{
    [[maybe_unused]] auto &a = pSource;
    [[maybe_unused]] auto &b = size;
    [[maybe_unused]] auto &c = fileName;
    return 1;
}

int File::saveToMemory(const void *pSource, const size_t &size, const QString &fileName) const
{
    [[maybe_unused]] auto &a = pSource;
    [[maybe_unused]] auto &b = size;
    [[maybe_unused]] auto &c = fileName;
    return 1;
}
} // namespace CAO
