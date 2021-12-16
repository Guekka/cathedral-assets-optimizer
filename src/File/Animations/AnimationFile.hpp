/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "File/File.hpp"

namespace CAO {
class AnimationFile : public File
{
public:
    int loadFromDisk(const QString &filePath) override;
    int saveToDisk(const QString &filePath) override;

    int loadFromMemory(const void *pSource, size_t size, const QString &fileName) override;
    int saveToMemory(std::vector<std::byte> &out) override;

    bool setFile(Resource &&file, bool optimizedFile = true) override;

    CommandType type() const override { return CommandType::Animation; }
};
} // namespace CAO
