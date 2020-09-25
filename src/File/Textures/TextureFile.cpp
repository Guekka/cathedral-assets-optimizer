/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "TextureFile.hpp"
#include "Utils/ScopeGuard.hpp"
#include <QDir>

namespace CAO {

TextureFile::TextureFile()
{
    // Initialize COM (needed for WIC)
    const HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
        throw std::runtime_error(QString("Failed to initialize COM. Textures processing won't work. hr: '%1'")
                                     .arg(QString::number(hr, 16))
                                     .toStdString());
}

int TextureFile::loadFromDisk(const QString &filePath)
{
    ScopeGuard guard([this] { this->reset(); });

    loadHelper<Resources::Texture>(filePath);

    wchar_t wFilePath[1024];
    QDir::toNativeSeparators(filePath).toWCharArray(wFilePath);
    wFilePath[filePath.length()] = '\0';

    //Trying to guess texture type. DDS is more common
    auto *image = getFile<Resources::Texture>(false);
    if (FAILED(LoadFromDDSFile(wFilePath, DirectX::DDS_FLAGS_NONE, &_info, *image)))
        if (FAILED(LoadFromTGAFile(wFilePath, &_info, *image)))
            return 1;

    if (!makeTypelessUNORM(*image))
        return 2;
    image->origFormat = _info.format;

    guard.clear();
    return 0;
}

int TextureFile::loadFromMemory(const void *pSource, size_t size, const QString &fileName)
{
    ScopeGuard guard([this] { this->reset(); });

    loadHelper<Resources::Texture>(fileName);

    auto *image = getFile<Resources::Texture>(false);
    if (FAILED(LoadFromDDSMemory(pSource, size, DirectX::DDS_FLAGS_NONE, &_info, *image)))
        if (FAILED(LoadFromTGAMemory(pSource, size, &_info, *image)))
            return 1;

    if (!makeTypelessUNORM(*image))
        return 2;
    image->origFormat = _info.format;

    guard.clear();
    return 0;
}

int TextureFile::saveToMemory(std::vector<std::byte> &out)
{
    if (!saveToMemoryHelper())
        return 1;

    const auto *image = getFile<Resources::Texture>();

    const auto img = image->GetImages();
    if (!img)
        return 2;
    const size_t nimg = image->GetImageCount();

    DirectX::Blob blob;
    if (FAILED(DirectX::SaveToDDSMemory(img, nimg, _info, DirectX::DDS_FLAGS_NONE, blob)))
        return 3;

    std::byte *data = static_cast<std::byte *>(blob.GetBufferPointer());
    size_t size     = blob.GetBufferSize();

    out.reserve(size);
    std::move(data, data + size, std::back_inserter(out));

    return 0;
}

int TextureFile::saveToDisk(const QString &filePath)
{
    if (!saveToDiskHelper(filePath))
        return 2;

    const auto *image = getFile<Resources::Texture>();
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
        QFile(getInputFilePath()).remove();
    return FAILED(hr);
}

bool TextureFile::setFile(Resource &&file, bool optimizedFile)
{
    if (!setFileHelper<Resources::Texture>(std::move(file), optimizedFile))
        return false;
    _info = getFile<Resources::Texture>()->GetMetadata();
    return true;
}

void TextureFile::reset()
{
    resetHelper();
    _info = DirectX::TexMetadata();
}

bool TextureFile::makeTypelessUNORM(Resources::Texture &image)
{
    if (DirectX::IsTypeless(_info.format))
    {
        _info.format = DirectX::MakeTypelessUNORM(_info.format);
        if (DirectX::IsTypeless(_info.format))
            return false;

        image.OverrideFormat(_info.format);
    }
    return true;
}
} // namespace CAO
