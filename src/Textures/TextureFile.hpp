/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "File.hpp"
#include "pch.hpp"

namespace CAO {
using DXScratchImagePtr = std::unique_ptr<DirectX::ScratchImage>;

class TextureFile final : public File
{
public:
    TextureFile();

    int loadFromDisk(const QString &filePath) override;
    int saveToDisk(const QString &filePath) const override;

    int loadFromMemory(const void *pSource, const size_t &size, const QString &fileName) override;

    const DirectX::ScratchImage &getFile() const;
    void setFile(DirectX::ScratchImage &file);
    void setFile(DXScratchImagePtr &file);

    void setFileUnmodified(DXScratchImagePtr &file);
    void setFileUnmodified(DirectX::ScratchImage &file);

    void reset() override;

    bool isTGA() const { return _isTGA; }

private:
    DXScratchImagePtr _image;
    DirectX::TexMetadata _info;
    bool _isTGA;
};
} // namespace CAO
