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

bool TexturesOptimizer::resize(size_t targetWidth, size_t targetHeight)
{
    
}

bool TexturesOptimizer::canHaveMipMaps()
{
    return !((_name.contains("interface", Qt::CaseInsensitive) && !Profiles::texturesCompressInterface())
             || DirectX::IsCompressed(_info.format) || _info.width < 4 || _info.height < 4);
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
