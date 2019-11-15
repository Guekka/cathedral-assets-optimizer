/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "TextureResize.hpp"

namespace CAO {
CommandResult TextureResize::process(File &file, const OptionsCAO &options)
{
    auto texFile = dynamic_cast<const TextureResource *>(&file);
    if (!texFile)
        return _resultFactory.getCannotCastFileResult();

    const auto &info = texFile->GetMetadata();

    auto timage = std::make_unique<TextureResource>();
    const auto img = texFile->GetImages();
    if (!img)
        return _resultFactory.getFailedResult(1, "Failed to get images from file");

    const auto &tinfo = calculateTargetDimensions(info, options);

    const DWORD filter = DirectX::TEX_FILTER_FANT | DirectX::TEX_FILTER_SEPARATE_ALPHA;
    const HRESULT hr = Resize(img, texFile->GetImageCount(), info, tinfo.width, tinfo.height, filter, *timage);
    if (FAILED(hr))
        return _resultFactory.getFailedResult(2, "Failed to resize image");

    file.setFile(*timage.release());
    return _resultFactory.getSuccessfulResult();
}

bool TextureResize::isApplicable(File &file, const OptionsCAO &options)
{
    if (!options.bTexturesResizeSize || !options.bTexturesResizeSize)
        return false;

    auto texFile = dynamic_cast<const TextureResource *>(&file);
    if (!texFile)
        return false;

    const DXGI_FORMAT fileFormat = texFile->GetMetadata().format;
    if (DirectX::IsCompressed(fileFormat))
        return false; //Cannot process compressed file

    const auto info = texFile->GetMetadata();
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
