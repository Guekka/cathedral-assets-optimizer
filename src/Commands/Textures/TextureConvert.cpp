/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "TextureConvert.hpp"

namespace CAO {
CommandResult TextureConvert::process(File &file)
{
    auto texFile = dynamic_cast<const TextureResource *>(&file.getFile());
    if (!texFile)
        return _resultFactory.getCannotCastFileResult();

    auto outputFormat = file.patternSettings().eTexturesFormat();

    //Textures smaller than that cannot be compressed
    if (texFile->GetMetadata().width < 4 || texFile->GetMetadata().height < 4)
        outputFormat = DXGI_FORMAT_B8G8R8A8_UNORM; //Standard uncompressed format

    auto timage = new TextureResource;

    if (DirectX::IsCompressed(outputFormat))
    {
        if (auto result = convertWithCompression(*texFile, *timage, outputFormat))
            return _resultFactory.getFailedResult(result, "Failed to convert with compression");
    }
    else
    {
        if (auto result = convertWithoutCompression(*texFile, *timage, outputFormat))
            return _resultFactory.getFailedResult(result, "Failed to convert without compression");
    }

    file.setFile(*timage);
    return _resultFactory.getSuccessfulResult();
}

bool TextureConvert::isApplicable(File &file)
{
    auto texResource = dynamic_cast<const TextureResource *>(&file.getFile());
    if (!texResource)
        return false;

    const DXGI_FORMAT &origFormat    = texResource->origFormat;
    const DXGI_FORMAT &currentFormat = texResource->GetMetadata().format;
    if (DirectX::IsCompressed(currentFormat))
        return false; //Cannot process compressed file

    //If the target format is the same as the current format, no conversion is needed
    if (file.patternSettings().eTexturesFormat() == currentFormat)
        return false;

    const bool sameFormatAsOrig = currentFormat == origFormat;

    //Compatible but compressing in order to improve performance
    const bool userWantsConvert = file.patternSettings().bTexturesCompress()
                                  && sameFormatAsOrig; //If true, the file was not compressed originally

    const bool necessary = needsConvert(file, currentFormat);

    //If the file was optimized, it was previously in a different format, and thus needs a conversion
    const bool optimizedFile = file.optimizedCurrentFile();

    return necessary || userWantsConvert || optimizedFile;
}

bool TextureConvert::needsConvert(const File &file, DXGI_FORMAT format)
{
    //Checking incompatibility with file format
    const auto &pSets         = file.patternSettings();
    const auto &vec           = pSets.slTextureUnwantedFormats();
    const bool isUnwanted     = std::find(vec.begin(), vec.end(), format) != vec.end();
    const bool isIncompatible = isUnwanted || pSets.bTexturesForceConvert();
    const bool needsConvert   = pSets.bTexturesNecessary() && isIncompatible;

    return needsConvert;
}

int TextureConvert::convertWithoutCompression(const DirectX::ScratchImage &image,
                                              DirectX::ScratchImage &timage,
                                              const DXGI_FORMAT &format)
{
    const HRESULT hr = Convert(image.GetImages(),
                               image.GetImageCount(),
                               image.GetMetadata(),
                               format,
                               0,
                               DirectX::TEX_THRESHOLD_DEFAULT,
                               timage);
    return FAILED(hr);
}

int TextureConvert::convertWithCompression(const DirectX::ScratchImage &image,
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
    TextureCompressionDevice compressionDevice;
    if (bc6hbc7 && compressionDevice.isValid())
        hr = DirectX::Compress(compressionDevice.getDevice(),
                               img,
                               nimg,
                               image.GetMetadata(),
                               format,
                               DirectX::TEX_COMPRESS_FLAGS::TEX_COMPRESS_BC7_USE_3SUBSETS,
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

    return FAILED(hr);
}
} // namespace CAO
