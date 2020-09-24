/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "TextureConvert.hpp"
#include "Commands/Textures/TextureCompressionDevice.hpp"
#include "File/Textures/TextureFile.hpp"

namespace CAO {
CommandResult TextureConvert::process(File &file) const
{
    auto texFile = dynamic_cast<const TextureResource *>(&file.getFile());
    if (!texFile)
        return CommandResultFactory::getCannotCastFileResult();

    auto outputFormat = file.patternSettings().eTexturesFormat();

    //Textures smaller than that cannot be compressed
    if (texFile->GetMetadata().width < 4 || texFile->GetMetadata().height < 4)
        outputFormat = DXGI_FORMAT_B8G8R8A8_UNORM; //Standard uncompressed format

    auto timage = std::make_unique<TextureResource>();

    if (DirectX::IsCompressed(outputFormat))
    {
        if (auto result = convertWithCompression(*texFile, *timage, outputFormat))
            return CommandResultFactory::getFailedResult(result, "Failed to convert with compression");
    }
    else
    {
        if (auto result = convertWithoutCompression(*texFile, *timage, outputFormat))
            return CommandResultFactory::getFailedResult(result, "Failed to convert without compression");
    }

    file.setFile(std::unique_ptr<Resource>(std::move(timage)));
    return CommandResultFactory::getSuccessfulResult();
}

CommandState TextureConvert::isApplicable(File &file) const
{
    auto texResource = dynamic_cast<const TextureResource *>(&file.getFile());
    if (!texResource)
        return CommandState::NotRequired;

    //Cannot process very small textures
    if (std::min(texResource->GetMetadata().width, texResource->GetMetadata().height) < 2)
        return CommandState::NotRequired;

    const DXGI_FORMAT origFormat    = texResource->origFormat;
    const DXGI_FORMAT currentFormat = texResource->GetMetadata().format;
    //If the target format is the same as the current format, no conversion is needed
    if (file.patternSettings().eTexturesFormat() == currentFormat)
        return CommandState::NotRequired;

    const bool necessary = needsConvert(file.patternSettings(), texResource->GetMetadata());
    if (necessary && DirectX::IsCompressed(currentFormat))
        return CommandState::PendingPreviousSteps;
    else if (necessary)
        return CommandState::Ready;

    //Compatible but compressing in order to improve performance
    const bool userWantsConvert = file.patternSettings().bTexturesCompress()
                                  && !DirectX::IsCompressed(origFormat);

    if (userWantsConvert)
        return CommandState::Ready; //Cannot process compressed file

    //If current format != original format, the file was optimized and may need to be recompressed
    if (currentFormat != origFormat)
        return CommandState::Ready;

    return CommandState::NotRequired;
}

bool TextureConvert::needsConvert(const PatternSettings &pSets, const DirectX::TexMetadata &info)
{
    const auto format = info.format;
    //Checking incompatibility with file format
    const auto &vec           = pSets.slTextureUnwantedFormats();
    const bool isUnwanted     = std::find(vec.begin(), vec.end(), format) != vec.end();
    const bool isIncompatible = isUnwanted || pSets.bTexturesForceConvert();

    const bool isCubemap    = info.IsCubemap();
    const bool uncompressed = !DirectX::IsCompressed(format);
    const bool hasAlpha     = DirectX::HasAlpha(format);

    const bool needsConvertCubemaps = isCubemap && uncompressed && !hasAlpha;
    const bool needsConvert         = pSets.bTexturesNecessary() && isIncompatible;

    return needsConvert || needsConvertCubemaps;
}

HRESULT TextureConvert::convertWithoutCompression(const DirectX::ScratchImage &image,
                                                  DirectX::ScratchImage &timage,
                                                  const DXGI_FORMAT &format)
{
    const HRESULT hr = Convert(image.GetImages(),
                               image.GetImageCount(),
                               image.GetMetadata(),
                               format,
                               DirectX::TEX_FILTER_SEPARATE_ALPHA,
                               DirectX::TEX_THRESHOLD_DEFAULT,
                               timage);

    return hr;
}

HRESULT TextureConvert::convertWithCompression(const DirectX::ScratchImage &image,
                                               DirectX::ScratchImage &timage,
                                               const DXGI_FORMAT &format)
{
    const auto img = image.GetImages();
    if (!img)
        return 1;
    const size_t nimg = image.GetImageCount();

    bool bc6hbc7 = false;
    switch (format)
    {
        case DXGI_FORMAT_BC6H_TYPELESS:
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC6H_SF16:
        case DXGI_FORMAT_BC7_TYPELESS:
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB: bc6hbc7 = true; break;
        default: bc6hbc7 = false;
    }

    HRESULT hr;
    static TextureCompressionDevice compressionDevice(getProfiles().commonSettings().iGPUIndex());
    if (bc6hbc7 && compressionDevice.isValid())
        hr = DirectX::Compress(compressionDevice.getDevice(),
                               img,
                               nimg,
                               image.GetMetadata(),
                               format,
                               DirectX::TEX_COMPRESS_FLAGS::TEX_COMPRESS_BC7_USE_3SUBSETS
                                   | DirectX::TEX_FILTER_SEPARATE_ALPHA,
                               DirectX::TEX_THRESHOLD_DEFAULT,
                               timage);
    else
        hr = DirectX::Compress(img,
                               nimg,
                               image.GetMetadata(),
                               format,
                               DirectX::TEX_COMPRESS_DEFAULT | DirectX::TEX_FILTER_SEPARATE_ALPHA,
                               DirectX::TEX_THRESHOLD_DEFAULT,
                               timage);

    return hr;
}
} // namespace CAO
