/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "TextureFile.hpp"

namespace CAO {

TextureFile::TextureFile()
{
    _file = std::make_unique<TextureResource>();
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
    auto image = static_cast<TextureResource *>(&*_file);
    if (FAILED(LoadFromDDSFile(wFilePath, DirectX::DDS_FLAGS_NONE, &_info, *image)))
    {
        if (FAILED(LoadFromTGAFile(wFilePath, &_info, *image)))
        {
            reset();
            return 1;
        }
        image->isTGA = true;
    }

    if (DirectX::IsTypeless(_info.format))
    {
        _info.format = DirectX::MakeTypelessUNORM(_info.format);
        if (DirectX::IsTypeless(_info.format))
        {
            reset();
            return 2;
        }
        image->OverrideFormat(_info.format);
    }
    image->origFormat = _info.format;

    _filename = filePath;
    return 0;
}

int TextureFile::loadFromMemory(const void *pSource, const size_t &size, const QString &fileName)
{
    reset();
    PLOG_VERBOSE << "Loading from memory " << fileName;
    _filename = fileName;

    auto image = static_cast<TextureResource *>(&*_file);
    if (FAILED(LoadFromDDSMemory(pSource, size, DirectX::DDS_FLAGS_NONE, &_info, *image)))
        if (FAILED(LoadFromTGAMemory(pSource, size, &_info, *image)))
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
        image->OverrideFormat(_info.format);
    }

    _filename = fileName;
    return 0;
}

int TextureFile::saveToDisk(const QString &filePath) const
{
    auto image = static_cast<TextureResource *>(&*_file);
    const auto img = image->GetImages();
    if (!img)
        return 1;
    const size_t nimg = image->GetImageCount();

    QString newName = filePath;
    if (image->isTGA)
        newName = newName.replace(".tga", ".dds");

    // Write texture
    wchar_t wFilePath[1024];
    QDir::toNativeSeparators(newName).toWCharArray(wFilePath);
    wFilePath[filePath.length()] = '\0';

    const HRESULT hr = SaveToDDSFile(img, nimg, _info, DirectX::DDS_FLAGS_NONE, wFilePath);
    if (SUCCEEDED(hr) && image->isTGA)
        QFile(_filename).remove();
    return FAILED(hr);
}

bool TextureFile::setFile(Resource &file, bool optimizedFile)
{
    auto image = dynamic_cast<TextureResource *>(&file);
    if (!image)
        return false;

    _file.reset(&file);
    _optimizedCurrentFile |= optimizedFile;
    _info = image->GetMetadata();
    return true;
}

void TextureFile::reset()
{
    _filename.clear();
    _file.reset(new TextureResource);
    _info = DirectX::TexMetadata();
    _optimizedCurrentFile = false;
}
} // namespace CAO
