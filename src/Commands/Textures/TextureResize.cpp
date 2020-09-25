/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "TextureResize.hpp"
#include "File/Textures/TextureFile.hpp"

namespace CAO {
CommandResult TextureResize::process(File &file) const
{
    const auto *texFile = file.getFile<Resources::Texture>();
    if (!texFile)
        return CommandResultFactory::getCannotCastFileResult();

    const auto &info = texFile->GetMetadata();

    Resources::Texture timage;
    const auto &img = texFile->GetImages();
    if (!img)
        return CommandResultFactory::getFailedResult(-1, "Failed to get images from file");

    const auto &tinfo = calculateTargetDimensions(info, file.patternSettings());

    const auto filter = DirectX::TEX_FILTER_SEPARATE_ALPHA;
    const HRESULT hr
        = DirectX::Resize(img, texFile->GetImageCount(), info, tinfo.width, tinfo.height, filter, timage);
    if (FAILED(hr))
        return CommandResultFactory::getFailedResult(hr, "Failed to resize image.");

    file.setFile(std::move(timage));
    return CommandResultFactory::getSuccessfulResult();
}

CommandState TextureResize::isApplicable(File &file) const
{
    if (file.patternSettings().eTexturesResizingMode() == None)
        return CommandState::NotRequired;

    const auto *texFile = file.getFile<Resources::Texture>();
    if (!texFile)
        return CommandState::NotRequired;

    const auto &info  = texFile->GetMetadata();
    const auto &tinfo = calculateTargetDimensions(info, file.patternSettings());

    if (info.width == tinfo.width && info.height == tinfo.height)
        return CommandState::NotRequired;

    const DXGI_FORMAT fileFormat = texFile->GetMetadata().format;
    if (DirectX::IsCompressed(fileFormat))
        return CommandState::PendingPreviousSteps; //Cannot process compressed file

    return CommandState::Ready;
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
        while (wMultiplier < settings.iTexturesResizingWidth()
               && hMultiplier < settings.iTexturesResizingHeight()
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
        while (tinfo.width > settings.iTexturesResizingWidth()
               && tinfo.height > settings.iTexturesResizingHeight())
        {
            tinfo.width /= 2;
            tinfo.height /= 2;
        }
    }
    return tinfo;
}
} // namespace CAO
