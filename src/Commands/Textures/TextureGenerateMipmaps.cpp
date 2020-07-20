/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "TextureGenerateMipmaps.hpp"

namespace CAO {
CommandResult TextureGenerateMipmaps::process(File &file) const
{
    auto texFile = dynamic_cast<const TextureResource *>(&file.getFile());
    if (!texFile)
        return _resultFactory.getCannotCastFileResult();

    const auto &info    = texFile->GetMetadata();
    const size_t &tMips = calculateOptimalMipMapsNumber(info);
    auto timage         = std::make_unique<TextureResource>();

    DirectX::TexMetadata mdata = info;
    mdata.mipLevels            = 1;
    const auto hr1             = timage->Initialize(mdata);
    if (FAILED(hr1))
        return _resultFactory.getFailedResult(1, "Failed to initialize target image with source metadata");

    // Mips generation only works on a single base image, so strip off existing mip levels
    // Also required for preserve alpha coverage so that existing mips are regenerated

    for (size_t i = 0; i < info.arraySize; ++i)
    {
        const auto hr = CopyRectangle(*texFile->GetImage(0, i, 0),
                                      DirectX::Rect(0, 0, info.width, info.height),
                                      *timage->GetImage(0, i, 0),
                                      DirectX::TEX_FILTER_SEPARATE_ALPHA,
                                      0,
                                      0);
        if (FAILED(hr))
            return _resultFactory.getFailedResult(1, "Failed to copy image to single level");
    }

    auto timage2        = std::make_unique<TextureResource>();
    timage2->origFormat = texFile->origFormat;

    const auto hr = GenerateMipMaps(timage->GetImages(),
                                    timage->GetImageCount(),
                                    timage->GetMetadata(),
                                    DirectX::TEX_FILTER_SEPARATE_ALPHA,
                                    tMips,
                                    *timage2);

    if (FAILED(hr))
        return _resultFactory.getFailedResult(2, "Failed to generate mipmaps: " + QString::number(hr, 16));

    file.setFile(std::move(timage2));
    return _resultFactory.getSuccessfulResult();
}

bool TextureGenerateMipmaps::isApplicable(File &file) const
{
    if (!file.patternSettings().bTexturesMipmaps())
        return false;

    auto texFile = dynamic_cast<const TextureResource *>(&file.getFile());
    if (!texFile)
        return false;

    const auto &info = texFile->GetMetadata();
    if (DirectX::IsCompressed(info.format))
        return false; //Cannot process compressed file

    const bool &compatible     = info.width >= 4 && info.height >= 4;
    const bool &optimalMipMaps = info.mipLevels == calculateOptimalMipMapsNumber(info);

    return compatible && !optimalMipMaps;
}

size_t TextureGenerateMipmaps::calculateOptimalMipMapsNumber(const DirectX::TexMetadata &info) const
{
    if (info.width < 4 && info.height < 4)
    {
        return 1;
    }

    size_t height = info.height;
    size_t width  = info.width;
    size_t tMips  = 1;
    //Calculating mips levels
    while (height > 1 && width > 1)
    {
        height /= 2;
        width /= 2;
        ++tMips;
    }
    return tMips;
}
} // namespace CAO
