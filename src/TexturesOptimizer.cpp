/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "TexturesOptimizer.h"


HRESULT TexturesOptimizer::open(const QString& filePath, const TextureType& type)
{
    PLOG_VERBOSE << "Opening " << filePath << "with textures type" << type;

    wchar_t fileName[1024];
    filePath.toWCharArray(fileName);

    name = filePath;

    image.reset(new (std::nothrow) DirectX::ScratchImage);
    if (!image)
        return E_OUTOFMEMORY;

    switch (type)
    {
    case tga:
        return LoadFromTGAFile(fileName, &info, *image);
    case dds:
        DWORD ddsFlags = DirectX::DDS_FLAGS_NONE;
        HRESULT hr = LoadFromDDSFile(fileName, ddsFlags, &info, *image);
        if (FAILED(hr))
            return false;

        if (DirectX::IsTypeless(info.format))
        {
            info.format = DirectX::MakeTypelessUNORM(info.format);

            if (DirectX::IsTypeless(info.format))
                return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);

            image->OverrideFormat(info.format);
        }

        return S_OK;
    }
    return S_FALSE;
}

HRESULT TexturesOptimizer::open(const void* pSource, const size_t &size, const TextureType &type, const QString& fileName)
{
    PLOG_VERBOSE << "Opening " << fileName << " from memory with textures type " << type;
    name = fileName;

    image.reset(new (std::nothrow) DirectX::ScratchImage);
    if (!image)
        return E_OUTOFMEMORY;

    switch (type)
    {
    case tga:
        return LoadFromTGAMemory(pSource, size, &info, *image);
    case dds:
        DWORD ddsFlags = DirectX::DDS_FLAGS_NONE;
        HRESULT hr = LoadFromDDSMemory(pSource, size, ddsFlags, &info, *image);
        if (FAILED(hr))
            return false;

        if (DirectX::IsTypeless(info.format))
        {
            info.format = DirectX::MakeTypelessUNORM(info.format);

            if (DirectX::IsTypeless(info.format))
                return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);

            image->OverrideFormat(info.format);
        }
        return S_OK;
    }
    return S_FALSE;
}

void TexturesOptimizer::compress() //Compress uncompressed textures to BC7
{
    if(isCompressed())
        return;

    PLOG_INFO << tr("Compressing uncompressed texture: ") + name;
            //convert(CAO_TEXTURES_FORMAT);
}

bool TexturesOptimizer::convertIncompatibleTextures() //Convert TGA textures to DDS
{/*
    if(isIncompatible(filePath))
    {
        PLOG_INFO << tr("Incompatible texture found: ")
                     + filePath.mid(filePath.lastIndexOf("/")) + "\n" + tr("Compressing...");

        convert(CAO_TEXTURES_FORMAT);

        return true;
    }
    else
    */    return false;
}


bool TexturesOptimizer::isCompressed()
{
    return DirectX::IsCompressed(info.format);
}

DirectX::TexMetadata TexturesOptimizer::getInfo()
{
    return info;
}

void TexturesOptimizer::convert(const DXGI_FORMAT& format)
{

}

bool TexturesOptimizer::isIncompatible()
{
    bool isIncompatible = false;

    //TODO replace the cao extensions list with a tga checkbox
    //TODO interface textures cannot be compressed for some games

    //Checking incompatibility with file format
    DXGI_FORMAT fileFormat = info.format;
    for(const auto& format : CAO_TEXTURES_INCOMPATIBLE_FORMATS)
        if(fileFormat == format)
        {
            isIncompatible = true;
            break;
        }

    return isIncompatible;
}
