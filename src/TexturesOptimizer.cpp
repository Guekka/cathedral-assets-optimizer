/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "TexturesOptimizer.h"

TexturesOptimizer::TexturesOptimizer()
{
    PLOG_WARNING_IF(!createDevice(0, pDevice.GetAddressOf()))
            << "DirectCompute is not available, using BC6H / BC7 CPU codec."
               " Textures compression will be slower";
}

bool TexturesOptimizer::GetDXGIFactory(IDXGIFactory1** pFactory)
{
    if (!pFactory)
        return false;

    *pFactory = nullptr;

    typedef HRESULT(WINAPI* pfn_CreateDXGIFactory1)(REFIID riid, _Out_ void **ppFactory);

    static pfn_CreateDXGIFactory1 s_CreateDXGIFactory1 = nullptr;

    if (!s_CreateDXGIFactory1)
    {
        HMODULE hModDXGI = LoadLibraryW(L"dxgi.dll");
        if (!hModDXGI)
            return false;

        s_CreateDXGIFactory1 = reinterpret_cast<pfn_CreateDXGIFactory1>(reinterpret_cast<void*>(GetProcAddress(hModDXGI, "CreateDXGIFactory1")));
        if (!s_CreateDXGIFactory1)
            return false;
    }

    return SUCCEEDED(s_CreateDXGIFactory1(IID_PPV_ARGS(pFactory)));
}


bool TexturesOptimizer::createDevice(int adapter, ID3D11Device** pDevice)
{
    if (!pDevice)
        return false;

    *pDevice = nullptr;

    static PFN_D3D11_CREATE_DEVICE s_DynamicD3D11CreateDevice = nullptr;

    if (!s_DynamicD3D11CreateDevice)
    {
        HMODULE hModD3D11 = LoadLibraryW(L"d3d11.dll");
        if (!hModD3D11)
            return false;

        s_DynamicD3D11CreateDevice = reinterpret_cast<PFN_D3D11_CREATE_DEVICE>(reinterpret_cast<void*>(GetProcAddress(hModD3D11, "D3D11CreateDevice")));
        if (!s_DynamicD3D11CreateDevice)
            return false;
    }

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

    UINT createDeviceFlags = 0;

    Microsoft::WRL::ComPtr<IDXGIAdapter> pAdapter;
    if (adapter >= 0)
    {
        Microsoft::WRL::ComPtr<IDXGIFactory1> dxgiFactory;
        if (GetDXGIFactory(dxgiFactory.GetAddressOf()))
        {
            if (FAILED(dxgiFactory->EnumAdapters(adapter, pAdapter.GetAddressOf())))
            {
                wprintf(L"\nERROR: Invalid GPU adapter index (%d)!\n", adapter);
                return false;
            }
        }
    }

    D3D_FEATURE_LEVEL fl;
    HRESULT hr = s_DynamicD3D11CreateDevice(pAdapter.Get(),
                                            (pAdapter) ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE,
                                            nullptr, createDeviceFlags, featureLevels, _countof(featureLevels),
                                            D3D11_SDK_VERSION, pDevice, &fl, nullptr);
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
    else
        return false;
}

bool TexturesOptimizer::optimize(const int& optLevel)
{
    if(optLevel < 1)
        return true;

    if(isCompressed())
    {
        if(!decompress())
            return false;
    }

    if(!convertToCompatibleFormat())
        return false;

    //Fitting to a power of two
    if(!resize(info.width, info.height))
        return false;

    if(optLevel >= 3)
    {
        if(!generateMipMaps())
            return false;
    }

    if(optLevel >= 2 && canBeCompressed())
    {
        if(!compress(CAO_TEXTURES_FORMAT))
            return false;
    }
    return true;
}

bool TexturesOptimizer::canBeCompressed()
{
    if(name.contains("interface", Qt::CaseInsensitive) && !CAO_TEXTURES_COMPRESS_INTERFACE)
        return false;

    return true;
}

bool TexturesOptimizer::open(const QString& filePath, const TextureType& type)
{
    PLOG_VERBOSE << "Opening " << filePath << "with textures type " << type;

    wchar_t fileName[1024];
    QDir::toNativeSeparators(filePath).toWCharArray(fileName);
    fileName[filePath.length()] = '\0';

    name = filePath;

    image.reset(new (std::nothrow) DirectX::ScratchImage);
    if (!image)
        return false;

    HRESULT hr;
    switch (type)
    {
    case tga:
        hr = LoadFromTGAFile(fileName, &info, *image);
        return SUCCEEDED(hr);
    case dds:
        DWORD ddsFlags = DirectX::DDS_FLAGS_NONE;
        hr = LoadFromDDSFile(fileName, ddsFlags, &info, *image);
        if (FAILED(hr))
            return false;

        if (DirectX::IsTypeless(info.format))
        {
            info.format = DirectX::MakeTypelessUNORM(info.format);

            if (DirectX::IsTypeless(info.format))
                return false;

            image->OverrideFormat(info.format);
        }
        return true;
    }
    return false;
}

bool TexturesOptimizer::open(const void* pSource, const size_t &size, const TextureType &type, const QString& fileName)
{
    PLOG_VERBOSE << "Opening " << fileName << " from memory with textures type " << type;
    name = fileName;

    image.reset(new (std::nothrow) DirectX::ScratchImage);
    if (!image)
        return false;

    switch (type)
    {
    case tga:
        return LoadFromTGAMemory(pSource, size, &info, *image);
    case dds:
        DWORD ddsFlags = DirectX::DDS_FLAGS_NONE;
        HRESULT hr = LoadFromDDSMemory(pSource, size, ddsFlags, &info, *image);
        if (FAILED(hr))
            return false;

        if (DirectX::IsTypeless(info.format))
        {
            info.format = DirectX::MakeTypelessUNORM(info.format);

            if (DirectX::IsTypeless(info.format))
                return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);

            image->OverrideFormat(info.format);
        }
        return true;
    }
    return false;
}

bool TexturesOptimizer::compress(const DXGI_FORMAT &format)
{
    if(isCompressed() || image->GetMetadata().format == format
            || !DirectX::IsCompressed(format))
        return true;

    PLOG_INFO << tr("Compressing texture: ") + name;

    auto img = image->GetImage(0, 0, 0);
    if(!img)
        return false;
    size_t nimg = image->GetImageCount();

    std::unique_ptr<DirectX::ScratchImage> timage(new (std::nothrow) DirectX::ScratchImage);
    if (!timage)
    {
        PLOG_ERROR << tr("Memory allocation failed when compressing: ") + name;
        return false;
    }

    bool bc6hbc7 = false;
    if(format == DXGI_FORMAT_BC6H_TYPELESS || format == DXGI_FORMAT_BC6H_UF16
            || format == DXGI_FORMAT_BC6H_SF16 || format == DXGI_FORMAT_BC7_TYPELESS
            || format == DXGI_FORMAT_BC7_UNORM || format == DXGI_FORMAT_BC7_UNORM_SRGB)
    {
        bc6hbc7 = true;
    }

    HRESULT hr;
    if (bc6hbc7 && pDevice)
        hr = Compress(pDevice.Get(), img, nimg, info, format, DirectX::TEX_COMPRESS_FLAGS::TEX_COMPRESS_BC7_USE_3SUBSETS, 1.f, *timage);
    else
        hr = Compress(img, nimg, info, format, DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, *timage);

    if (FAILED(hr))
    {
        PLOG_ERROR << "Failed to compress: " + name;
        return false;
    }

    const auto& tinfo = timage->GetMetadata();
    info.format = tinfo.format;

    if(!compareInfo(info, tinfo))
        return false;

    image.swap(timage);
    return true;
}

bool TexturesOptimizer::decompress()
{
    if (!DirectX::IsCompressed(info.format))
        return false;

    auto img = image->GetImage(0, 0, 0);
    assert(img);
    size_t nimg = image->GetImageCount();

    std::unique_ptr<DirectX::ScratchImage> timage(new (std::nothrow) DirectX::ScratchImage);
    if (!timage)
    {
        PLOG_ERROR << "Memory allocation failed";
        return false;
    }

    HRESULT hr = Decompress(img, nimg, info, DXGI_FORMAT_UNKNOWN /* picks good default */, *timage);
    if (FAILED(hr))
    {
        PLOG_ERROR << "Failed to decompress: " << name;
        return false;
    }

    const auto& tinfo = timage->GetMetadata();
    info.format = tinfo.format;

    if(!compareInfo(info, tinfo))
        return false;

    image.swap(timage);
    return true;
}

bool TexturesOptimizer::resize(size_t targetWidth, size_t targetHeight)
{
    if (info.width == targetWidth && info.height == targetHeight)
        return true;

    fitPowerOfTwo(targetWidth, targetHeight);

    std::unique_ptr<DirectX::ScratchImage> timage(new (std::nothrow) DirectX::ScratchImage);
    if (!timage)
    {
        PLOG_ERROR << "Memory allocation failed";
        return false;
    }

    HRESULT hr = Resize(image->GetImages(), image->GetImageCount(), image->GetMetadata(), targetWidth, targetHeight, DirectX::TEX_FILTER_DEFAULT, *timage);
    if (FAILED(hr))
    {
        PLOG_ERROR << "Failed to resize: " + name;
        return false;
    }

    auto& tinfo = timage->GetMetadata();

    assert(tinfo.width == targetWidth && tinfo.height == targetHeight && tinfo.mipLevels == 1);
    info.width = tinfo.width;
    info.height = tinfo.height;
    info.mipLevels = 1;

    if(!compareInfo(info, tinfo))
        return false;

    image.swap(timage);
    return true;
}


bool TexturesOptimizer::generateMipMaps()
{
    size_t tMips = 1;
    size_t height = info.height;
    size_t width = info.width;
    //Calculating mips levels
    while (height > 1 || width > 1)
    {
        if (height > 1)
            height >>= 1;

        if (width > 1)
            width >>= 1;

        ++tMips;
    }

    if (info.mipLevels != 1 && info.mipLevels != tMips)
    {
        // Mips generation only works on a single base image, so strip off existing mip levels
        // Also required for preserve alpha coverage so that existing mips are regenerated
        std::unique_ptr<DirectX::ScratchImage> timage(new (std::nothrow) DirectX::ScratchImage);
        if (!timage)
        {
            PLOG_ERROR << "Memory allocation failed";
            return false;
        }

        DirectX::TexMetadata mdata = info;
        mdata.mipLevels = 1;
        HRESULT hr = timage->Initialize(mdata);
        if (FAILED(hr))
        {
            PLOG_ERROR << "Failed to copy texture data to single level (when generating mipmaps) when processing: " << name;
            return false;
        }

        for (size_t i = 0; i < info.arraySize; ++i)
        {
            hr = CopyRectangle(*image->GetImage(0, i, 0), DirectX::Rect(0, 0, info.width, info.height),
                               *timage->GetImage(0, i, 0), DirectX::TEX_FILTER_DEFAULT, 0, 0);
            if (FAILED(hr))
            {
                PLOG_ERROR << "Failed to copy texture data to single level (when generating mipmaps) when processing: " << name;
                return false;
            }
        }

        image.swap(timage);
        info.mipLevels =image->GetMetadata().mipLevels;
    }

    if ((info.width > 1 || info.height > 1 || info.depth > 1))
    {
        std::unique_ptr<DirectX::ScratchImage> timage(new (std::nothrow) DirectX::ScratchImage);
        if(!timage)
        {
            PLOG_ERROR << "Memory allocation failed";
            return false;
        }

        //Forcing non wic since WIC won't work on my computer, and thus probably on other computers
        const HRESULT hr =
                GenerateMipMaps(image->GetImages(),image->GetImageCount(),image->GetMetadata(),
                                DirectX::TEX_FILTER_FORCE_NON_WIC, info.mipLevels, *timage);
        if(FAILED(hr))
        {
            PLOG_ERROR << "Failed to generate mipmaps when processing: " << name;
            return false;
        }

        const auto& tinfo = timage->GetMetadata();
        info.mipLevels = tinfo.mipLevels;

        if(!compareInfo(info, tinfo))
            return false;

        image.swap(timage);
    }
    return true;
}

bool TexturesOptimizer::convertToCompatibleFormat()
{
    if(isIncompatible())
    {
        PLOG_INFO << tr("Incompatible texture found: ") + name + '\n' + tr("Compressing...");
        return convert(CAO_TEXTURES_FORMAT);
    }
    return true;
}


bool TexturesOptimizer::isCompressed()
{
    return DirectX::IsCompressed(info.format);
}

DirectX::TexMetadata TexturesOptimizer::getInfo()
{
    return info;
}

bool TexturesOptimizer::convert(const DXGI_FORMAT& format)
{
    if(!image)
    {
        PLOG_ERROR << "Texture isn't opened: " + name;
        return false;
    }

    if (info.format != format && !DirectX::IsCompressed(format))
    {
        std::unique_ptr<DirectX::ScratchImage> timage(new (std::nothrow) DirectX::ScratchImage);
        if (!timage)
        {
            PLOG_ERROR << "Memory allocation failed";
            return false;
        }

        HRESULT hr = Convert(image->GetImages(), image->GetImageCount(), image->GetMetadata(), format,
                             0, DirectX::TEX_THRESHOLD_DEFAULT, *timage);
        if (FAILED(hr))
        {
            PLOG_ERROR << "Failed to convert: " + name;
            return false;
        }

        const auto& tinfo = timage->GetMetadata();
        if(tinfo.format != format)
            return false;
        info.format = tinfo.format;

        if(!compareInfo(info, tinfo))
            return false;

        image.swap(timage);
    }
    return true;
}

bool TexturesOptimizer::saveToFile(const QString &filePath)
{
    auto img = image->GetImage(0, 0, 0);
    assert(img);
    size_t nimg = image->GetImageCount();

    // Write texture
    wchar_t wFilePath[1024];
    QDir::toNativeSeparators(filePath).toWCharArray(wFilePath);
    wFilePath[filePath.length()] = '\0';

    const HRESULT hr = SaveToDDSFile(img, nimg, info, DirectX::DDS_FLAGS_NONE, wFilePath);
    return SUCCEEDED(hr);
}

bool TexturesOptimizer::isIncompatible()
{
    //Checking incompatibility with file format
    const DXGI_FORMAT fileFormat = info.format;
    for(const auto& format : CAO_TEXTURES_INCOMPATIBLE_FORMATS)
        if(fileFormat == format)
            return true;

    return false;
}

void TexturesOptimizer::fitPowerOfTwo(uint& resultX, uint& resultY)
{
    //Finding nearest power of two
    resultX = info.width;
    uint x = 1;
    while(x < resultX)
        x *= 2;
    resultX = x;

    resultY = info.height;
    uint y = 1;
    while(y < resultY)
        y *= 2;
    resultY = y;
}

bool TexturesOptimizer::compareInfo(const DirectX::TexMetadata &info1, const DirectX::TexMetadata &info2)
{
    const bool isSame =
            info1.width == info2.width ||
            info1.height == info2.height ||
            info1.depth == info2.depth ||
            info1.arraySize == info2.arraySize ||
            info1.miscFlags == info2.miscFlags ||
            info1.format == info2.format ||
            info1.dimension == info2.dimension;

    return isSame;
}
