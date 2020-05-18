/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "TextureResize.hpp"

namespace CAO {
CommandResult TextureResize::process(File &file)
{
    auto texFile = dynamic_cast<const TextureResource *>(&file);
    if (!texFile)
        return _resultFactory.getCannotCastFileResult();

    const auto &info = texFile->GetMetadata();

    auto timage     = std::make_unique<TextureResource>();
    const auto &img = texFile->GetImages();
    if (!img)
        return _resultFactory.getFailedResult(1, "Failed to get images from file");

    const auto &tinfo = calculateTargetDimensions(info, file.patternSettings());

    const DWORD &filter = DirectX::TEX_FILTER_FANT | DirectX::TEX_FILTER_SEPARATE_ALPHA;
    const HRESULT &hr = Resize(img, texFile->GetImageCount(), info, tinfo.width, tinfo.height, filter, *timage);
    if (FAILED(hr))
        return _resultFactory.getFailedResult(2, "Failed to resize image");

    file.setFile(*timage);
    return _resultFactory.getSuccessfulResult();
}

bool TextureResize::isApplicable(File &file)
{
    if (file.patternSettings().eTexturesResizingMode() == None)
        return false;

    auto texFile = dynamic_cast<const TextureResource *>(&file);
    if (!texFile)
        return false;

    const DXGI_FORMAT &fileFormat = texFile->GetMetadata().format;
    if (DirectX::IsCompressed(fileFormat))
        return false; //Cannot process compressed file

    const auto &info  = texFile->GetMetadata();
    const auto &tinfo = calculateTargetDimensions(info, file.patternSettings());

    return info.width != tinfo.width || info.height != tinfo.height;
}

DirectX::TexMetadata TextureResize::calculateTargetDimensions(const DirectX::TexMetadata &info,
                                                              const PatternSettings &settings)
{
    //Calculating target width and height
    DirectX::TexMetadata tinfo = info;
    if (settings.eTexturesResizingMode() == ByRatio)
    {
        uint wMultiplier = 1;
        uint hMultiplier = 1;
        while (wMultiplier < settings.iTexturesResizingByRatioWidth()
               && hMultiplier < settings.iTexturesResizingByRatioHeight()
               && tinfo.width > settings.iTexturesMinimumWidth()
               && tinfo.height > settings.iTexturesMinimumHeight())
        {
            wMultiplier *= 2;
            hMultiplier *= 2;
            tinfo.width /= 2;
            tinfo.height /= 2;
        }
    }
    else if (settings.eTexturesResizingMode() == BySize)
    {
        while (tinfo.width > settings.iTexturesResizingBySizeWidth()
               && tinfo.height > settings.iTexturesResizingBySizeHeight())
        {
            tinfo.width /= 2;
            tinfo.height /= 2;
        }
    }
    return tinfo;
}
} // namespace CAO
