/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "File/File.hpp"
#include "pch.hpp"

namespace CAO {
class MeshFile : public File
{
public:
    int loadFromDisk(const QString &filePath) override;
    int saveToDisk(const QString &filePath) const override;

    int loadFromMemory(const void *pSource, size_t size, const QString &fileName) override;
    int saveToMemory(std::iostream &ostr) const override;

    bool setFile(std::unique_ptr<Resource> file, bool optimizedFile = false) override;

    CommandType type() override { return CommandType::Mesh; }
};
} // namespace CAO
