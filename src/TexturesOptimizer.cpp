/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "TexturesOptimizer.h"

TexturesOptimizer::TexturesOptimizer()
{
    PLOG_WARNING_IF(!createDevice(0, _pDevice.GetAddressOf()))
        << "DirectCompute is not available, using BC6H / BC7 CPU codec."
           " Textures compression will be slower";

    // Initialize COM (needed for WIC)
    const HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    if (FAILED(hr))
        throw std::runtime_error("Failed to initialize COM. Textures processing won't work.");
}

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

bool TexturesOptimizer::getDXGIFactory(IDXGIFactory1 **pFactory) const
{
    if (!pFactory)
        return false;

    *pFactory = nullptr;

    typedef HRESULT(WINAPI * pfn_CreateDXGIFactory1)(REFIID riid, _Out_ void **ppFactory);

    static pfn_CreateDXGIFactory1 sCreateDXGIFactory1 = nullptr;

    if (!sCreateDXGIFactory1)
    {
        const HMODULE hModDXGI = LoadLibraryW(L"dxgi.dll");
        if (!hModDXGI)
            return false;

        sCreateDXGIFactory1 = reinterpret_cast<pfn_CreateDXGIFactory1>(
            reinterpret_cast<void *>(GetProcAddress(hModDXGI, "CreateDXGIFactory1")));
        if (!sCreateDXGIFactory1)
            return false;
    }

    return SUCCEEDED(sCreateDXGIFactory1(IID_PPV_ARGS(pFactory)));
}

bool TexturesOptimizer::createDevice(const int adapter, ID3D11Device **pDevice) const
{
    if (!pDevice)
        return false;

    *pDevice = nullptr;

    static PFN_D3D11_CREATE_DEVICE s_DynamicD3D11CreateDevice = nullptr;

    if (!s_DynamicD3D11CreateDevice)
    {
        const HMODULE hModD3D11 = LoadLibraryW(L"d3d11.dll");
        if (!hModD3D11)
            return false;

        s_DynamicD3D11CreateDevice = reinterpret_cast<PFN_D3D11_CREATE_DEVICE>(
            reinterpret_cast<void *>(GetProcAddress(hModD3D11, "D3D11CreateDevice")));
        if (!s_DynamicD3D11CreateDevice)
            return false;
    }

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

    const UINT createDeviceFlags = 0;

    Microsoft::WRL::ComPtr<IDXGIAdapter> pAdapter;
    if (adapter >= 0)
    {
        Microsoft::WRL::ComPtr<IDXGIFactory1> dxgiFactory;
        if (getDXGIFactory(dxgiFactory.GetAddressOf()))
        {
            if (FAILED(dxgiFactory->EnumAdapters(static_cast<uint>(adapter), pAdapter.GetAddressOf())))
            {
                PLOG_ERROR << "ERROR: Invalid GPU adapter index: " << adapter;
                return false;
            }
        }
    }

    D3D_FEATURE_LEVEL fl;
    HRESULT hr = s_DynamicD3D11CreateDevice(pAdapter.Get(),
                                            (pAdapter) ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE,
                                            nullptr,
                                            createDeviceFlags,
                                            featureLevels,
                                            _countof(featureLevels),
                                            D3D11_SDK_VERSION,
                                            pDevice,
                                            &fl,
                                            nullptr);
    if (SUCCEEDED(hr))
    {
        if (fl < D3D_FEATURE_LEVEL_11_0)
        {
            D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts;
            hr = (*pDevice)->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts));
            if (FAILED(hr))
                memset(&hwopts, 0, sizeof(hwopts));

            if (!hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x)
            {
                if (*pDevice)
                {
                    (*pDevice)->Release();
                    *pDevice = nullptr;
                }
                hr = HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
            }
        }
    }

    if (SUCCEEDED(hr))
    {
        Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
        hr = (*pDevice)->QueryInterface(IID_PPV_ARGS(dxgiDevice.GetAddressOf()));

        return SUCCEEDED(hr);
    }
    return false;
}

TexturesOptimizer::TexOptOptionsResult TexturesOptimizer::processArguments(const bool &bNecessary,
                                                                           const bool &bCompress,
                                                                           const bool &bMipmaps,
                                                                           const std::optional<size_t> &tWidth,
                                                                           const std::optional<size_t> &tHeight)
{
    TexOptOptionsResult result;
    //Calculating target width and height

    result.tWidth = _info.width;
    result.tHeight = _info.height;
    if (tWidth.has_value() && tHeight.has_value())
    {
        while (result.tWidth > tWidth.value() && result.tHeight > tHeight.value())
        {
            result.tWidth /= 2;
            result.tHeight /= 2;
        }
    }

    result.bNeedsResize =
        result.tHeight != _info.height || result.tWidth != _info.width;

    result.bNeedsCompress = (bNecessary && (isIncompatible() || _type == TGA))
                            || (bCompress && canBeCompressed()
                                && _info.format != Profiles::texturesFormat());

    result.bNeedsMipmaps = bMipmaps && _info.mipLevels != calculateOptimalMipMapsNumber() && canHaveMipMaps();

    return result;
}

bool TexturesOptimizer::optimize(const bool &bNecessary,
                                 const bool &bCompress,
                                 const bool &bMipmaps,
                                 const std::optional<size_t> &tWidth,
                                 const std::optional<size_t> &tHeight)
{
    PLOG_VERBOSE << "Processing arguments for: " << _name;
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

    //Fitting to a power of two or resizing
    if (options.bNeedsResize)
    {
        PLOG_VERBOSE << "Resizing this texture.";
        if (!resize(options.tWidth, options.tHeight))
            return false;

        options.bNeedsMipmaps = bMipmaps && _info.mipLevels != calculateOptimalMipMapsNumber()
                                && canHaveMipMaps();
    }

    //Generating mipmaps
    if (options.bNeedsMipmaps)
    {
        PLOG_VERBOSE << "Generating mipmaps for this texture.";
        if (!generateMipMaps())
            return false;
    }

    //Converting to the new format, or the compressing back into the original format
    if (options.bNeedsCompress)
    {
        targetFormat = Profiles::texturesFormat();
        PLOG_VERBOSE << "Converting this texture to format: " << dxgiFormatToString(targetFormat);
    }

    // Cannot compress if size became smaller than 4x4
    if (!canBeCompressed())
        targetFormat = DXGI_FORMAT_B8G8R8A8_UNORM;

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
                                 || (bCompress && canBeCompressed()
                                     && _info.format != Profiles::texturesFormat());

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
    const bool isInterface = _name.contains("interface", Qt::CaseInsensitive);
    const bool already = DirectX::IsCompressed(_info.format);
    const bool badSize = _info.width < 4 || _info.height < 4;

    const bool interfaceOkay = Profiles::texturesCompressInterface() || !isInterface;

    return interfaceOkay && !already && !badSize;
}

bool TexturesOptimizer::open(const QString &filePath, const TextureType &type)
{
    PLOG_VERBOSE << "Opening " << filePath << " with textures type " << type;

    wchar_t fileName[1024];
    QDir::toNativeSeparators(filePath).toWCharArray(fileName);
    fileName[filePath.length()] = '\0';

    _image.reset(new (std::nothrow) DirectX::ScratchImage);
    if (!_image)
        return false;

    modifiedCurrentTexture = false;

    HRESULT hr = S_FALSE;
    switch (type)
    {
    case TGA: hr = LoadFromTGAFile(fileName, &_info, *_image); break;
    case DDS:
        const auto ddsFlags = DirectX::DDS_FLAGS_NONE;
        hr = LoadFromDDSFile(fileName, ddsFlags, &_info, *_image);
        if (FAILED(hr))
            return false;

        if (DirectX::IsTypeless(_info.format))
        {
            _info.format = DirectX::MakeTypelessUNORM(_info.format);

            if (DirectX::IsTypeless(_info.format))
                return false;

            _image->OverrideFormat(_info.format);
        }
    }
    if (SUCCEEDED(hr))
    {
        _type = type;
        _name = filePath;
        return true;
    }
    return false;
}

bool TexturesOptimizer::open(const void *pSource, const size_t &size, const TextureType &type, const QString &fileName)
{
    PLOG_VERBOSE << "Opening " << fileName << " from memory with textures type " << type;
    _name = fileName;

    _image.reset(new (std::nothrow) DirectX::ScratchImage);
    if (!_image)
        return false;

    modifiedCurrentTexture = false;

    switch (type)
    {
    case TGA: return LoadFromTGAMemory(pSource, size, &_info, *_image);
    case DDS:
        const auto ddsFlags = DirectX::DDS_FLAGS_NONE;
        const HRESULT hr = LoadFromDDSMemory(pSource, size, ddsFlags, &_info, *_image);
        if (FAILED(hr))
            return false;

        if (DirectX::IsTypeless(_info.format))
        {
            _info.format = DirectX::MakeTypelessUNORM(_info.format);

            if (DirectX::IsTypeless(_info.format))
                return false;

            _image->OverrideFormat(_info.format);
        }
        return true;
    }
    return false;
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

    // DirectX::Resize is dumb. If WIC is used, it will convert the image to
    // R32G32B32A32 It works for small image.. But will, for example, allocate
    // 1gb for a 8k picture. So disable WIC
    const auto filter = DirectX::TEX_FILTER_SEPARATE_ALPHA | DirectX::TEX_FILTER_FORCE_NON_WIC;
    const HRESULT hr = Resize(imgs, _image->GetImageCount(), _info, targetWidth,
                              targetHeight, filter, *timage);
    if (FAILED(hr))
    {
        PLOG_ERROR << QString("Failed to resize: '%1'. Error code: '%2'")
                          .arg(_name, QString::number(hr, 16));
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
    const bool isInterface = _name.contains("interface", Qt::CaseInsensitive);
    const bool interfaceOkay = !isInterface || Profiles::texturesCompressInterface();
    const bool sizeOkay = _info.width >= 4 && _info.height >= 4;

    return interfaceOkay && sizeOkay;
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
            const auto filter = DirectX::TEX_FILTER_SEPARATE_ALPHA;
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
        const auto filter = DirectX::TEX_FILTER_SEPARATE_ALPHA;
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
                                   DirectX::TEX_FILTER_FLAGS::TEX_FILTER_DEFAULT,
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

bool TexturesOptimizer::saveToFile(const QString &filePath) const
{
    const auto img = _image->GetImage(0, 0, 0);
    if (!img)
        return false;
    const size_t nimg = _image->GetImageCount();

    // Write texture
    wchar_t wFilePath[1024];
    QDir::toNativeSeparators(filePath).toWCharArray(wFilePath);
    wFilePath[filePath.length()] = '\0';

    const HRESULT hr = SaveToDDSFile(img, nimg, _info, DirectX::DDS_FLAGS_NONE, wFilePath);
    return SUCCEEDED(hr);
}

bool TexturesOptimizer::isIncompatible() const
{
    //Checking incompatibility with file format
    const DXGI_FORMAT fileFormat = _info.format;
    for (const auto &f : Profiles::texturesUnwantedFormats())
        if (f == fileFormat)
            return true;

    const bool isCubemap = _info.IsCubemap();
    const bool uncompressed = !DirectX::IsCompressed(fileFormat);

    const bool opaqueAlpha = _info.GetAlphaMode() == DirectX::TEX_ALPHA_MODE::TEX_ALPHA_MODE_OPAQUE;
    const bool noAlpha = !DirectX::HasAlpha(fileFormat);
    const bool badAlpha = opaqueAlpha || noAlpha;

    const bool badCubemap = isCubemap && uncompressed && badAlpha;

    return badCubemap;
}

void TexturesOptimizer::fitPowerOfTwo(size_t &resultX, size_t &resultY)
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
