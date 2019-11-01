/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"

namespace CAO {
class File
{
public:
    const QString &getName() const { return _filename; }
    void setName(const QString &name) { _filename = name; }

    virtual int loadFromDisk(const QString &filePath) = 0;
    virtual int loadFromMemory(const void *pSource, const size_t &size, const QString &fileName);

    virtual int saveToDisk(const QString &filePath) const = 0;
    virtual int saveToMemory(const void *pSource, const size_t &size, const QString &fileName) const;

    bool modifiedCurrentFile() const { return _modifiedCurrentFile; }

    virtual void reset() = 0;

    virtual ~File() = default;

protected:
    QString _filename;
    bool _modifiedCurrentFile;
};

} // namespace CAO
