/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "TextureConvert.hpp"

namespace CAO {
int TextureConvert::process(File &file, const OptionsCAO &options)
{
    auto texFile = dynamic_cast<TextureFile *>(&file);
    if (!texFile)
        return 3;

    const auto &outputFormat = Profiles::texturesFormat();

    std::any aTimage = new DirectX::ScratchImage;
    DirectX::ScratchImage &timage = std::any_cast<DirectX::ScratchImage &>(aTimage);

    int result = 1;
    if (DirectX::IsCompressed(outputFormat))
        result = convertWithCompression(texFile->getFile(), timage, outputFormat);
    else
        result = convertWithoutCompression(texFile->getFile(), timage, outputFormat);

    if (result)
        return 1;

    texFile->setFile(timage);
    return 0;
}

bool TextureConvert::isApplicable(File &file, const OptionsCAO &options)
{
    auto texFile = dynamic_cast<TextureFile *>(&file);
    if (!texFile)
        return false;

    //Checking incompatibility with file format
    const auto &unwantedFormats = Profiles::texturesUnwantedFormats();
    const DXGI_FORMAT fileFormat = texFile->getFile().GetMetadata().format;
    const bool isIncompatible = unwantedFormats.contains(fileFormat);

    //Truely incompatible
    const bool needsConvert = options.bTexturesNecessary && (isIncompatible || texFile->isTGA());
    //Compatible but compressing in order to improve performance
    const bool wantsConvert = options.bTexturesCompress && !DirectX::IsCompressed(fileFormat)
                              && !!DirectX::IsCompressed(Profiles::texturesFormat());

    return needsConvert || wantsConvert;
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
