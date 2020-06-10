/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "File/File.hpp"
#include "pch.hpp"

namespace CAO {

class TextureFile final : public File
{
public:
    TextureFile();

    int loadFromDisk(const QString &filePath) override;
    int saveToDisk(const QString &filePath) const override;

    int loadFromMemory(const void *pSource, const size_t &size, const QString &fileName) override;

    bool setFile(std::unique_ptr<Resource> file, bool optimizedFile = true) override;

    void reset() override;
private:
    DirectX::TexMetadata _info{};
};
} // namespace CAO
