/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "TextureFile.hpp"

TextureFile::TextureFile()
{
    // Initialize COM (needed for WIC)
    const HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
        throw std::runtime_error("Failed to initialize COM. Textures processing won't work.");
}

int TextureFile::open(const QString &filePath, const TextureType &type)
{
    PLOG_VERBOSE << "Opening " << filePath << " with textures type " << type;

    wchar_t fileName[1024];
    QDir::toNativeSeparators(filePath).toWCharArray(fileName);
    fileName[filePath.length()] = '\0';

    _image.reset(new (std::nothrow) DirectX::ScratchImage);
    if (!_image)
        return 3;

    _modifiedCurrentTexture = false;

    HRESULT hr = S_FALSE;
    switch (type)
    {
        case TextureType::TGA: hr = LoadFromTGAFile(fileName, &_info, *_image); break;
        case TextureType::DDS: hr = LoadFromDDSFile(fileName, DirectX::DDS_FLAGS_NONE, &_info, *_image); break;
    }

    if (FAILED(hr))
        return 1;

    if (DirectX::IsTypeless(_info.format))
    {
        _info.format = DirectX::MakeTypelessUNORM(_info.format);

        if (DirectX::IsTypeless(_info.format))
            return 2;

        _image->OverrideFormat(_info.format);
    }

    if (SUCCEEDED(hr))
    {
        _type = type;
        _name = filePath;
        return 0;
    }
    return 1;
}

int TextureFile::open(const void *pSource, const size_t &size, const TextureType &type, const QString &fileName)
{
    PLOG_VERBOSE << "Opening " << fileName << " from memory with textures type " << type;
    _name = fileName;

    _image.reset(new (std::nothrow) DirectX::ScratchImage);
    if (!_image)
        return 3;

    _modifiedCurrentTexture = false;

    HRESULT hr = S_FALSE;
    switch (type)
    {
        case TextureType::TGA: hr = LoadFromTGAMemory(pSource, size, &_info, *_image); break;
        case TextureType::DDS: hr = LoadFromDDSMemory(pSource, size, DirectX::DDS_FLAGS_NONE, &_info, *_image); break;
    }
    if (FAILED(hr))
        return 1;

    if (DirectX::IsTypeless(_info.format))
    {
        _info.format = DirectX::MakeTypelessUNORM(_info.format);

        if (DirectX::IsTypeless(_info.format))
            return 2;

        _image->OverrideFormat(_info.format);
    }

    if (SUCCEEDED(hr))
    {
        _type = type;
        _name = fileName;
        return 0;
    }
    return 1;
}

int TextureFile::saveToFile(const QString &filePath) const
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

const DirectX::ScratchImage &TextureFile::image() const
{
    return *_image;
}

void TextureFile::setImage(DirectX::ScratchImage *image)
{
    _image.reset(image);
    _modifiedCurrentTexture = true;
    _info = _image->GetMetadata();
}

void TextureFile::setImage(std::unique_ptr<DirectX::ScratchImage> *image)
{
    _image = std::move(*image);
    _modifiedCurrentTexture = true;
    _info = _image->GetMetadata();
}
