/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"

class TextureFile
{
public:
    TextureFile();

    enum class TextureType
    {
        DDS,
        TGA
    };

    int open(const void *pSource, const size_t &size, const TextureType &type, const QString &fileName);
    int open(const QString &filePath, const TextureType &type);

    int saveToFile(const QString &filePath) const;

    void close();

    const DirectX::ScratchImage &image() const;
    void setImage(DirectX::ScratchImage *image);
    void setImage(const std::shared_ptr<DirectX::ScratchImage> &image);
    void setImage(std::unique_ptr<DirectX::ScratchImage> *image);

private:
    std::unique_ptr<DirectX::ScratchImage> _image;
    DirectX::TexMetadata _info;
    QString _name;
    TextureType _type;

    bool _modifiedCurrentTexture;
};

namespace plog
{
    inline Record &operator<<(Record &record, const TextureFile::TextureType &type)
    {
        switch (type)
        {
            case TextureFile::TextureType::DDS: return record << "DDS";
            case TextureFile::TextureType::TGA: return record << "TGA";
        }
        return record << "Unknown type";
    }
} // namespace plog
