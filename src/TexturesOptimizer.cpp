/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "TexturesOptimizer.hpp"
#include "TextureFile.hpp"
TexturesOptimizer::TexturesOptimizer() {}

void TexturesOptimizer::listLandscapeTextures(QDirIterator &it)
{
    QFile &&customHeadpartsFile = Profiles::getFile("customHeadparts.txt");
    _landscapeTextures = FilesystemOperations::readFile(customHeadpartsFile);

    if (_landscapeTextures.isEmpty())
    {
        PLOG_ERROR << "customHeadparts.txt not found. This can cause issue when optimizing meshes, as some headparts "
                      "won't be detected.";
    }

    for (const auto &plugin : FilesystemOperations::listPlugins(it))
        _landscapeTextures += PluginsOperations::listLandscapeTextures(plugin);

    for (auto &tex : _landscapeTextures)
        if (!tex.endsWith("_n.dds"))
            tex.insert(tex.size() - 4, "_n");

    _landscapeTextures.removeDuplicates();
}

TexturesOptimizer::TexOptOptionsResult TexturesOptimizer::processArguments(const bool &bNecessary,
                                                                           const bool &bCompress,
                                                                           const bool &bMipmaps,
                                                                           const std::optional<size_t> &tWidth,
                                                                           const std::optional<size_t> &tHeight)
{
    TexOptOptionsResult result;
    //Calculating target width and height

    if (tWidth.has_value() && tHeight.has_value() && _info.width > tWidth.value() && _info.height > tHeight.value())
    {
        do
        {
            result.tWidth = _info.width / 2;
            result.tHeight = _info.height / 2;
        } while (result.tWidth > tWidth.value() && result.tHeight > tHeight.value());
    }

    result.bNeedsResize = (bNecessary && !isPowerOfTwo())
                          || (result.tHeight != _info.height || result.tWidth != _info.width);

    result.bNeedsCompress = (bNecessary && (isIncompatible() || _type == TGA))
                            || (bCompress && canBeCompressed() && _info.format != Profiles::texturesFormat());

    result.bNeedsMipmaps = bMipmaps && _info.mipLevels != calculateOptimalMipMapsNumber() && canHaveMipMaps();

    return result;
}

bool TexturesOptimizer::optimize(const bool &bNecessary,
                                 const bool &bCompress,
                                 const bool &bMipmaps,
                                 const std::optional<size_t> &tWidth,
                                 const std::optional<size_t> &tHeight)
{
    //Getting operations to perform. This will be repeated several times, since the texture will change after each operation
    auto options = processArguments(bNecessary, bCompress, bMipmaps, tWidth, tHeight);

    DXGI_FORMAT targetFormat = _info.format;

    PLOG_VERBOSE << "Processing texture: " << _name;

    if (!options.bNeedsCompress && !options.bNeedsResize && !options.bNeedsMipmaps)
    {
        PLOG_VERBOSE << "This texture does not need optimization.";
        return true;
    }

    //Decompressing
    if (isCompressed())
    {
        PLOG_VERBOSE << "Decompressing this texture.";
        if (!decompress())
            return false;
    }
    options = processArguments(bNecessary, bCompress, bMipmaps, tWidth, tHeight);

    //Fitting to a power of two or resizing
    if (options.bNeedsResize)
    {
        PLOG_VERBOSE << "Resizing this texture.";
        if (!resize(options.tWidth, options.tHeight))
            return false;
    }
    options = processArguments(bNecessary, bCompress, bMipmaps, tWidth, tHeight);

    //Generating mipmaps
    if (options.bNeedsMipmaps)
    {
        PLOG_VERBOSE << "Generating mipmaps for this texture.";
        if (!generateMipMaps())
            return false;
    }
    options = processArguments(bNecessary, bCompress, bMipmaps, tWidth, tHeight);

    //Converting to the new format, or the compressing back into the original format
    if (options.bNeedsCompress)
    {
        targetFormat = Profiles::texturesFormat();
        PLOG_VERBOSE << "Converting this texture to format: " << dxgiFormatToString(targetFormat);
    }
    if (!convert(targetFormat))
        return false;

    PLOG_INFO << "Successfully processed texture: " + _name;
    return true;
}

void TexturesOptimizer::dryOptimize(const bool &bNecessary,
                                    const bool &bCompress,
                                    const bool &bMipmaps,
                                    const std::optional<size_t> &tWidth,
                                    const std::optional<size_t> &tHeight)
{
    const size_t newWidth = tWidth.has_value() ? tWidth.value() : _info.width;
    const size_t newHeight = tHeight.has_value() ? tHeight.value() : _info.height;

    const bool needsResize = bNecessary && (!isPowerOfTwo() || newHeight != _info.height || newWidth != _info.width);

    const bool needsConversion = (bNecessary && (isIncompatible() || _type == TGA))
                                 || (bCompress && canBeCompressed() && _info.format != Profiles::texturesFormat());

    const bool needsMipMaps = bMipmaps && _info.mipLevels != calculateOptimalMipMapsNumber() && canHaveMipMaps();

    PLOG_INFO << "Analyzing texture: " << _name;

    if (!needsConversion && !needsMipMaps && !needsResize)
    {
        PLOG_VERBOSE << "This texture does not need optimization.";
    }

    //Fitting to a power of two or resizing
    if (needsResize)
    {
        PLOG_VERBOSE << "This texture would be resized.";
    }

    if (needsMipMaps)
    {
        PLOG_VERBOSE << "This texture would have mipmaps generated.";
    }

    //Converting or compressing to the new format
    if (needsConversion)
    {
        PLOG_VERBOSE << "This texture would be converted to format: " << dxgiFormatToString(Profiles::texturesFormat());
    }
}

bool TexturesOptimizer::canBeCompressed() const
{
    return !((_name.contains("interface", Qt::CaseInsensitive) && !Profiles::texturesCompressInterface())
             || DirectX::IsCompressed(_info.format) || _info.width < 4 || _info.height < 4);
}

bool TexturesOptimizer::decompress()
{
    if (!DirectX::IsCompressed(_info.format))
        return false;

    const auto img = _image->GetImage(0, 0, 0);
    assert(img);
    const size_t nimg = _image->GetImageCount();

    std::unique_ptr<DirectX::ScratchImage> timage(new (std::nothrow) DirectX::ScratchImage);
    if (!timage)
    {
        PLOG_ERROR << "Memory allocation failed";
        return false;
    }

    const HRESULT hr = Decompress(img, nimg, _info, DXGI_FORMAT_UNKNOWN /* picks good default */, *timage);
    if (FAILED(hr))
    {
        PLOG_ERROR << "Failed to decompress: " << _name;
        return false;
    }

    const auto &tinfo = timage->GetMetadata();
    _info.format = tinfo.format;

    if (!compareInfo(_info, tinfo))
        return false;

    _image.swap(timage);
    modifiedCurrentTexture = true;
    return true;
}

bool TexturesOptimizer::resize(size_t targetWidth, size_t targetHeight)
{
    if (_info.width <= targetWidth && _info.height <= targetHeight)
        return true;

    fitPowerOfTwo(targetWidth, targetHeight);

    std::unique_ptr<DirectX::ScratchImage> timage(new (std::nothrow) DirectX::ScratchImage);
    if (!timage)
    {
        PLOG_ERROR << "Memory allocation failed";
        return false;
    }

    const auto imgs = _image->GetImages();
    if (!imgs)
        return false;

    const DWORD filter = DirectX::TEX_FILTER_FANT | DirectX::TEX_FILTER_SEPARATE_ALPHA;
    const HRESULT hr = Resize(imgs, _image->GetImageCount(), _info, targetWidth, targetHeight, filter, *timage);
    if (FAILED(hr))
    {
        PLOG_ERROR << "Failed to resize: " + _name;
        return false;
    }

    auto &tinfo = timage->GetMetadata();

    assert(tinfo.width == targetWidth && tinfo.height == targetHeight && tinfo.mipLevels == 1);
    _info.width = tinfo.width;
    _info.height = tinfo.height;
    _info.mipLevels = 1;

    if (!compareInfo(_info, tinfo))
        return false;

    _image.swap(timage);
    modifiedCurrentTexture = true;
    return true;
}

bool TexturesOptimizer::canHaveMipMaps()
{
    return !((_name.contains("interface", Qt::CaseInsensitive) && !Profiles::texturesCompressInterface())
             || DirectX::IsCompressed(_info.format) || _info.width < 4 || _info.height < 4);
}

bool TexturesOptimizer::generateMipMaps()
{
    const size_t tMips = calculateOptimalMipMapsNumber();

    if (_info.mipLevels != 1 && _info.mipLevels != tMips)
    {
        // Mips generation only works on a single base image, so strip off existing mip levels
        // Also required for preserve alpha coverage so that existing mips are regenerated
        std::unique_ptr<DirectX::ScratchImage> timage(new (std::nothrow) DirectX::ScratchImage);
        if (!timage)
        {
            PLOG_ERROR << "Memory allocation failed";
            return false;
        }

        DirectX::TexMetadata mdata = _info;
        mdata.mipLevels = 1;
        HRESULT hr = timage->Initialize(mdata);
        if (FAILED(hr))
        {
            PLOG_ERROR << "Failed to copy texture data to single level (when generating mipmaps) when processing: "
                       << _name;
            return false;
        }

        for (size_t i = 0; i < _info.arraySize; ++i)
        {
            const DWORD filter = DirectX::TEX_FILTER_FANT | DirectX::TEX_FILTER_SEPARATE_ALPHA;
            hr = CopyRectangle(*_image->GetImage(0, i, 0),
                               DirectX::Rect(0, 0, _info.width, _info.height),
                               *timage->GetImage(0, i, 0),
                               filter,
                               0,
                               0);
            if (FAILED(hr))
            {
                PLOG_ERROR << "Failed to copy texture data to single level (when generating mipmaps) when processing: "
                           << _name;
                return false;
            }
        }

        modifiedCurrentTexture = true;
        _image.swap(timage);
        _info.mipLevels = _image->GetMetadata().mipLevels;
    }

    if ((_info.width > 1 || _info.height > 1 || _info.depth > 1))
    {
        std::unique_ptr<DirectX::ScratchImage> timage(new (std::nothrow) DirectX::ScratchImage);
        if (!timage)
        {
            PLOG_ERROR << "Memory allocation failed";
            return false;
        }

        //Forcing non wic since WIC won't work on my computer, and thus probably on other computers
        const DWORD filter = DirectX::TEX_FILTER_FANT | DirectX::TEX_FILTER_SEPARATE_ALPHA;
        const HRESULT hr = GenerateMipMaps(_image->GetImages(),
                                           _image->GetImageCount(),
                                           _image->GetMetadata(),
                                           filter,
                                           tMips,
                                           *timage);
        if (FAILED(hr))
        {
            PLOG_ERROR << "Failed to generate mipmaps when processing: " << _name;
            return false;
        }

        const auto &tinfo = timage->GetMetadata();
        _info.mipLevels = tinfo.mipLevels;

        if (!compareInfo(_info, tinfo))
            return false;

        _image.swap(timage);
        modifiedCurrentTexture = true;
    }
    return true;
}

size_t TexturesOptimizer::calculateOptimalMipMapsNumber() const
{
    size_t height = _info.height;
    size_t width = _info.width;
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

bool TexturesOptimizer::isCompressed() const
{
    return DirectX::IsCompressed(_info.format);
}

DirectX::TexMetadata TexturesOptimizer::getInfo() const
{
    return _info;
}

bool TexturesOptimizer::convert(const DXGI_FORMAT &format)
{
    if (DirectX::IsCompressed(format))
        return convertWithCompression(format);
    return convertWithoutCompression(format);
}

bool TexturesOptimizer::convertWithoutCompression(const DXGI_FORMAT &format)
{
    if (_info.format != format)
    {
        std::unique_ptr<DirectX::ScratchImage> timage(new (std::nothrow) DirectX::ScratchImage);
        if (!timage)
        {
            PLOG_ERROR << "Memory allocation failed";
            return false;
        }

        const HRESULT hr = Convert(_image->GetImages(),
                                   _image->GetImageCount(),
                                   _image->GetMetadata(),
                                   format,
                                   0,
                                   DirectX::TEX_THRESHOLD_DEFAULT,
                                   *timage);
        if (FAILED(hr))
        {
            PLOG_ERROR << "Failed to convert: " + _name;
            return false;
        }

        const auto &tinfo = timage->GetMetadata();
        if (tinfo.format != format)
            return false;
        _info.format = tinfo.format;

        if (!compareInfo(_info, tinfo))
            return false;

        _image.swap(timage);
        modifiedCurrentTexture = true;
    }
    return true;
}

bool TexturesOptimizer::convertWithCompression(const DXGI_FORMAT &format)
{
    if (isCompressed() || _image->GetMetadata().format == format)
        return true;

    const auto img = _image->GetImage(0, 0, 0);
    if (!img)
        return false;
    const size_t nimg = _image->GetImageCount();

    std::unique_ptr<DirectX::ScratchImage> timage(new (std::nothrow) DirectX::ScratchImage);
    if (!timage)
    {
        PLOG_ERROR << "Memory allocation failed when compressing: " + _name;
        return false;
    }

    bool bc6hbc7 = false;
    if (format == DXGI_FORMAT_BC6H_TYPELESS || format == DXGI_FORMAT_BC6H_UF16 || format == DXGI_FORMAT_BC6H_SF16
        || format == DXGI_FORMAT_BC7_TYPELESS || format == DXGI_FORMAT_BC7_UNORM
        || format == DXGI_FORMAT_BC7_UNORM_SRGB)
    {
        bc6hbc7 = true;
    }

    HRESULT hr;
    if (bc6hbc7 && _pDevice)
        hr = Compress(_pDevice.Get(),
                      img,
                      nimg,
                      _info,
                      format,
                      DirectX::TEX_COMPRESS_FLAGS::TEX_COMPRESS_BC7_USE_3SUBSETS,
                      1.f,
                      *timage);
    else
        hr = Compress(img,
                      nimg,
                      _info,
                      format,
                      DirectX::TEX_COMPRESS_DEFAULT | DirectX::TEX_FILTER_SEPARATE_ALPHA,
                      DirectX::TEX_THRESHOLD_DEFAULT,
                      *timage);

    if (FAILED(hr))
    {
        PLOG_ERROR << "Failed to compress: " + _name;
        return false;
    }

    const auto &tinfo = timage->GetMetadata();
    _info.format = tinfo.format;

    if (!compareInfo(_info, tinfo))
        return false;

    _image.swap(timage);
    modifiedCurrentTexture = true;
    return true;
}

bool TexturesOptimizer::isIncompatible() const
{
    //Checking incompatibility with file format
    const DXGI_FORMAT fileFormat = _info.format;
    for (const auto &f : Profiles::texturesUnwantedFormats())
        if (f == fileFormat)
            return true;
    return false;
}

void TexturesOptimizer::fitPowerOfTwo(uint &resultX, uint &resultY)
{
    //Finding nearest power of two
    uint x = 1;
    while (x < resultX)
        x *= 2;
    resultX = x;

    uint y = 1;
    while (y < resultY)
        y *= 2;
    resultY = y;
}

bool TexturesOptimizer::isPowerOfTwo() const
{
    return ((_info.width != 0) && !(_info.width & (_info.width - 1)))
           && ((_info.height != 0) && !(_info.height & (_info.height - 1)));
}

bool TexturesOptimizer::compareInfo(const DirectX::TexMetadata &info1, const DirectX::TexMetadata &info2)
{
    const bool isSame = info1.width == info2.width || info1.height == info2.height || info1.depth == info2.depth
                        || info1.arraySize == info2.arraySize || info1.miscFlags == info2.miscFlags
                        || info1.format == info2.format || info1.dimension == info2.dimension;

    return isSame;
}
