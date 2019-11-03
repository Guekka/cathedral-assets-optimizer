/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "TextureGenerateMipmaps.hpp"

namespace CAO {
CommandResult TextureGenerateMipmaps::process(File &file, const OptionsCAO &options)
{
    auto texFile = dynamic_cast<TextureFile *>(&file);
    if (!texFile)
        return _resultFactory.getCannotCastFileResult();

    const auto &image = texFile->getFile();
    const auto &info = image.GetMetadata();
    const size_t tMips = calculateOptimalMipMapsNumber(info);
    auto timage = std::make_unique<DirectX::ScratchImage>();

    if (info.mipLevels != 1 && info.mipLevels != tMips)
    {
        // Mips generation only works on a single base image, so strip off existing mip levels
        // Also required for preserve alpha coverage so that existing mips are regenerated

        DirectX::TexMetadata mdata = info;
        mdata.mipLevels = 1;
        HRESULT hr = timage->Initialize(mdata);
        if (FAILED(hr))
            return _resultFactory.getFailedResult(1, "Failed to initialize target image with source metadata");

        for (size_t i = 0; i < info.arraySize; ++i)
        {
            const DWORD filter = DirectX::TEX_FILTER_FANT | DirectX::TEX_FILTER_SEPARATE_ALPHA;
            hr = CopyRectangle(*image.GetImage(0, i, 0),
                               DirectX::Rect(0, 0, info.width, info.height),
                               *timage->GetImage(0, i, 0),
                               filter,
                               0,
                               0);
            if (FAILED(hr))
                return _resultFactory.getFailedResult(1, "Failed to copy image to single level");
        }
    }

    auto timage2 = std::make_unique<DirectX::ScratchImage>();
    const DWORD filter = DirectX::TEX_FILTER_FANT | DirectX::TEX_FILTER_SEPARATE_ALPHA;
    const auto hr
        = GenerateMipMaps(timage->GetImages(), timage->GetImageCount(), timage->GetMetadata(), filter, tMips, *timage2);

    if (FAILED(hr))
        return _resultFactory.getFailedResult(1, "Failed to generate mipmaps");

    texFile->setFile(timage2);
    return _resultFactory.getSuccessfulResult();
}

bool TextureGenerateMipmaps::isApplicable(File &file, const OptionsCAO &options)
{
    if (!options.bTexturesMipmaps)
        return false;

    auto texFile = dynamic_cast<TextureFile *>(&file);
    if (!texFile)
        return false;

    const DXGI_FORMAT fileFormat = texFile->getFile().GetMetadata().format;
    if (DirectX::IsCompressed(fileFormat))
        return false; //Cannot process compressed file

    const auto info = texFile->getFile().GetMetadata();
    const bool interfaceForbidden = false
                                    || (file.getName().contains("interface", Qt::CaseInsensitive)
                                        && !Profiles::texturesCompressInterface());

    const bool compatible = info.width >= 4 && info.height >= 4;
    const bool optimalMipMaps = info.mipLevels == calculateOptimalMipMapsNumber(info);

    return compatible && !interfaceForbidden && !optimalMipMaps;
}

size_t TextureGenerateMipmaps::calculateOptimalMipMapsNumber(const DirectX::TexMetadata &info) const
{
    size_t height = info.height;
    size_t width = info.width;
    size_t tMips = 1;
    //Calculating mips levels
    while (height > 1 || width > 1)
    {
        if (height > 1)
            height >>= 1;

        if (width > 1)
            width >>= 1;

        ++tMips;
    }
    return tMips;
}
} // namespace CAO
