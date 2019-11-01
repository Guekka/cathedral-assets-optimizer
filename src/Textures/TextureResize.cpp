/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "TextureResize.hpp"

namespace CAO {
int TextureResize::process(File &file, const OptionsCAO &options)
{
    auto texFile = dynamic_cast<TextureFile *>(&file);
    if (!texFile)
        return 3;

    const auto &image = texFile->getFile();
    const auto &info = image.GetMetadata();

    auto timage = std::make_unique<DirectX::ScratchImage>();
    const auto img = image.GetImages();
    if (!img)
        return false;

    const auto &tinfo = calculateTargetDimensions(info, options);

    const DWORD filter = DirectX::TEX_FILTER_FANT | DirectX::TEX_FILTER_SEPARATE_ALPHA;
    const HRESULT hr = Resize(img, image.GetImageCount(), info, tinfo.width, tinfo.height, filter, *timage);
    if (FAILED(hr))
        return 1;

    texFile->setFile(timage);
    return 0;
}

bool TextureResize::isApplicable(File &file, const OptionsCAO &options)
{
    if (!options.bTexturesResizeSize || !options.bTexturesResizeSize)
        return false;

    auto texFile = dynamic_cast<TextureFile *>(&file);
    if (!texFile)
        return false;

    const auto info = texFile->getFile().GetMetadata();
    const auto &tinfo = calculateTargetDimensions(info, options);

    return info.width != tinfo.width || info.height != tinfo.height;
}

DirectX::TexMetadata TextureResize::calculateTargetDimensions(const DirectX::TexMetadata &info,
                                                              const OptionsCAO &options)
{
    //Calculating target width and height
    DirectX::TexMetadata tinfo = info;
    if (options.bTexturesResizeRatio)
    {
        tinfo.width = info.width / options.iTexturesTargetWidthRatio;
        tinfo.height = info.height / options.iTexturesTargetHeightRatio;
    }
    else if (options.bTexturesResizeSize)
    {
        while (tinfo.width > options.iTexturesTargetWidth && tinfo.height > options.iTexturesTargetHeight)
        {
            tinfo.width = info.width / 2;
            tinfo.height = info.height / 2;
        }
    }
    return tinfo;
}
} // namespace CAO
