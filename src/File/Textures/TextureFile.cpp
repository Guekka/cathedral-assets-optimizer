/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "TextureFile.hpp"
#include "Utils/ScopeGuard.hpp"

namespace CAO {

TextureFile::TextureFile()
{
    reset();
    // Initialize COM (needed for WIC)
    const HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
        throw std::runtime_error("Failed to initialize COM. Textures processing won't work.");
}

int TextureFile::loadFromDisk(const QString &filePath)
{
    ScopeGuard guard([this] { this->reset(); });

    reset();
    setName(filePath);

    wchar_t wFilePath[1024];
    QDir::toNativeSeparators(filePath).toWCharArray(wFilePath);
    wFilePath[filePath.length()] = '\0';

    //Trying to guess texture type. DDS is more common
    auto image = static_cast<TextureResource *>(&getFile(false));
    if (FAILED(LoadFromDDSFile(wFilePath, DirectX::DDS_FLAGS_NONE, &_info, *image)))
        if (FAILED(LoadFromTGAFile(wFilePath, &_info, *image)))
            return 1;

    if (DirectX::IsTypeless(_info.format))
    {
        _info.format = DirectX::MakeTypelessUNORM(_info.format);
        if (DirectX::IsTypeless(_info.format))
            return 2;

        image->OverrideFormat(_info.format);
    }
    image->origFormat = _info.format;

    guard.clear();
    return 0;
}

int TextureFile::loadFromMemory(const void *pSource, const size_t &size, const QString &fileName)
{
    ScopeGuard guard([this] { this->reset(); });

    reset();
    setName(fileName);

    auto image = static_cast<TextureResource *>(&getFile(false));
    if (FAILED(LoadFromDDSMemory(pSource, size, DirectX::DDS_FLAGS_NONE, &_info, *image)))
        if (FAILED(LoadFromTGAMemory(pSource, size, &_info, *image)))
            return 1;

    if (DirectX::IsTypeless(_info.format))
    {
        _info.format = DirectX::MakeTypelessUNORM(_info.format);
        if (DirectX::IsTypeless(_info.format))
            return 2;

        image->OverrideFormat(_info.format);
    }

    guard.clear();
    return 0;
}

int TextureFile::saveToDisk(const QString &filePath) const
{
    auto image = static_cast<const TextureResource *>(&getFile());
    const auto img = image->GetImages();
    if (!img)
        return 1;
    const size_t nimg = image->GetImageCount();

    QString newName = filePath;
    bool isTGA      = false;
    if (newName.contains(".tga", Qt::CaseInsensitive))
    {
        isTGA = true;
        newName.replace(".tga", ".dds", Qt::CaseInsensitive);
    }

    // Write texture
    wchar_t wFilePath[1024];
    QDir::toNativeSeparators(newName).toWCharArray(wFilePath);
    wFilePath[filePath.length()] = '\0';

    const HRESULT hr = SaveToDDSFile(img, nimg, _info, DirectX::DDS_FLAGS_NONE, wFilePath);
    if (SUCCEEDED(hr) && isTGA)
        QFile(getName()).remove();
    return FAILED(hr);
}

bool TextureFile::setFile(Resource &file, bool optimizedFile)
{
    if (!setFileHelper<TextureResource>(file, optimizedFile))
        return false;
    _info = static_cast<TextureResource *>(&file)->GetMetadata();
    return true;
}

void TextureFile::reset()
{
    resetHelper<TextureResource>();
    _info = DirectX::TexMetadata();
}
} // namespace CAO
