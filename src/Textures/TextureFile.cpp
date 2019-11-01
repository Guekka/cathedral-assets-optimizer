/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "TextureFile.hpp"

namespace CAO {

TextureFile::TextureFile()
{
    // Initialize COM (needed for WIC)
    const HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
        throw std::runtime_error("Failed to initialize COM. Textures processing won't work.");
}

int TextureFile::loadFromDisk(const QString &filePath)
{
    reset();
    PLOG_VERBOSE << "Loading from disk " << filePath;

    wchar_t wFilePath[1024];
    QDir::toNativeSeparators(filePath).toWCharArray(wFilePath);
    wFilePath[filePath.length()] = '\0';

    //Trying to guess texture type. DDS is more common
    if (FAILED(LoadFromDDSFile(wFilePath, DirectX::DDS_FLAGS_NONE, &_info, *_image)))
    {
        if (FAILED(LoadFromTGAFile(wFilePath, &_info, *_image)))
        {
            reset();
            return 1;
        }
        _isTGA = true;
    }

    if (DirectX::IsTypeless(_info.format))
    {
        _info.format = DirectX::MakeTypelessUNORM(_info.format);
        if (DirectX::IsTypeless(_info.format))
        {
            reset();
            return 2;
        }
        _image->OverrideFormat(_info.format);
    }

    _filename = filePath;
    return 0;
}

int TextureFile::loadFromMemory(const void *pSource, const size_t &size, const QString &fileName)
{
    reset();
    PLOG_VERBOSE << "Loading from memory " << fileName;
    _filename = fileName;

    if (FAILED(LoadFromDDSMemory(pSource, size, DirectX::DDS_FLAGS_NONE, &_info, *_image)))
        if (FAILED(LoadFromTGAMemory(pSource, size, &_info, *_image)))
        {
            reset();
            return 1;
        }

    if (DirectX::IsTypeless(_info.format))
    {
        _info.format = DirectX::MakeTypelessUNORM(_info.format);
        if (DirectX::IsTypeless(_info.format))
        {
            reset();
            return 2;
        }
        _image->OverrideFormat(_info.format);
    }

    _filename = fileName;
    return 0;
}

int TextureFile::saveToDisk(const QString &filePath) const
{
    const auto img = _image->GetImages();
    if (!img)
        return 1;
    const size_t nimg = _image->GetImageCount();

    // Write texture
    wchar_t wFilePath[1024];
    QDir::toNativeSeparators(filePath).toWCharArray(wFilePath);
    wFilePath[filePath.length()] = '\0';

    const HRESULT hr = SaveToDDSFile(img, nimg, _info, DirectX::DDS_FLAGS_NONE, wFilePath);
    return FAILED(hr);
}

const DirectX::ScratchImage &TextureFile::getFile() const
{
    return *_image;
}

void TextureFile::setFile(DirectX::ScratchImage &file)
{
    _image.reset(&file);
    _modifiedCurrentFile = true;
    _info = _image->GetMetadata();
}

void TextureFile::setFile(DXScratchImagePtr &file)
{
    _image = std::move(file);
    _modifiedCurrentFile = true;
    _info = _image->GetMetadata();
}

void TextureFile::setFileUnmodified(DirectX::ScratchImage &file)
{
    _image.reset(&file);
    _info = _image->GetMetadata();
}

void TextureFile::setFileUnmodified(DXScratchImagePtr &file)
{
    _image = std::move(file);
    _info = _image->GetMetadata();
}

void TextureFile::reset()
{
    _filename.clear();
    _image.reset(new DirectX::ScratchImage);
    _info = DirectX::TexMetadata();
    _modifiedCurrentFile = false;
}
} // namespace CAO
