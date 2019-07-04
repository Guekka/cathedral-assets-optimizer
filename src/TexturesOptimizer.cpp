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
        if (SUCCEEDED(hr))
        {
            hr = dxgiDevice->GetAdapter(pAdapter.ReleaseAndGetAddressOf());
            if (SUCCEEDED(hr))
            {
                DXGI_ADAPTER_DESC desc;
                hr = pAdapter->GetDesc(&desc);
                if (SUCCEEDED(hr))
                {
                    wprintf(L"\n[Using DirectCompute on \"%ls\"]\n", desc.Description);
                }
            }
        }

        return true;
    }
    else
        return false;
}


HRESULT TexturesOptimizer::open(const QString& filePath, const TextureType& type)
{
    PLOG_VERBOSE << "Opening " << filePath << "with textures type" << type;

    wchar_t fileName[1024];
    filePath.toWCharArray(fileName);

    name = filePath;

    image.reset(new (std::nothrow) DirectX::ScratchImage);
    if (!image)
        return E_OUTOFMEMORY;

    HRESULT hr;
    switch (type)
    {
    case tga:
        hr = LoadFromTGAFile(fileName, &info, *image);

        return hr;

        break;
    case dds:
        DWORD ddsFlags = DirectX::DDS_FLAGS_NONE;
        hr = LoadFromDDSFile(fileName, ddsFlags, &info, *image);
        if (FAILED(hr))
            return hr;

        if (DirectX::IsTypeless(info.format))
        {
            info.format = DirectX::MakeTypelessUNORM(info.format);

            if (DirectX::IsTypeless(info.format))
                return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);

            image->OverrideFormat(info.format);
        }

        return hr;
    }
    return S_FALSE;
}

HRESULT TexturesOptimizer::open(const void* pSource, const size_t &size, const TextureType &type, const QString& fileName)
{
    PLOG_VERBOSE << "Opening " << fileName << " from memory with textures type " << type;
    name = fileName;

    image.reset(new (std::nothrow) DirectX::ScratchImage);
    if (!image)
        return E_OUTOFMEMORY;

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
        return S_OK;
    }
    return S_FALSE;
}

void TexturesOptimizer::compress(const DXGI_FORMAT &format)
{
    if(isCompressed())
        return; //TODO uncompress

    if (image->GetMetadata().format == format)
        return;

    if (!DirectX::IsCompressed(format))
        return;

    PLOG_INFO << tr("Compressing uncompressed texture: ") + name;

    auto img = image->GetImage(0, 0, 0);
    assert(img);
    size_t nimg = image->GetImageCount();

    std::unique_ptr<DirectX::ScratchImage> timage(new (std::nothrow) DirectX::ScratchImage);
    if (!timage)
    {
        PLOG_ERROR << "Memory allocation failed: " + name;
        return;
    }

    bool bc6hbc7 = false;
    if(format == DXGI_FORMAT_BC6H_TYPELESS || format == DXGI_FORMAT_BC6H_UF16
            || format == DXGI_FORMAT_BC6H_SF16 || format == DXGI_FORMAT_BC7_TYPELESS
            || format == DXGI_FORMAT_BC7_TYPELESS || format == DXGI_FORMAT_BC7_UNORM
            || format == DXGI_FORMAT_BC7_UNORM_SRGB)
    {
        bc6hbc7 = true;
    }

    DWORD cflags = DirectX::TEX_COMPRESS_DEFAULT;

    HRESULT hr;
    if (bc6hbc7 && pDevice)
        hr = Compress(pDevice.Get(), img, nimg, info, format, DirectX::TEX_COMPRESS_FLAGS::TEX_COMPRESS_BC7_USE_3SUBSETS, 1.f, *timage);
    else
        hr = Compress(img, nimg, info, format, cflags, DirectX::TEX_THRESHOLD_DEFAULT, *timage);

    if (FAILED(hr))
        wprintf(L" FAILED [compress] (%x)\n", hr);

    auto& tinfo = timage->GetMetadata();

    info.format = tinfo.format;
    image.swap(timage);

    //convert(CAO_TEXTURES_FORMAT);
}

int TexturesOptimizer::decompress()
{
    std::unique_ptr<DirectX::ScratchImage> cimage;

    if (!DirectX::IsCompressed(info.format))
        return 2;

    auto img = image->GetImage(0, 0, 0);
    assert(img);
    size_t nimg = image->GetImageCount();

    std::unique_ptr<DirectX::ScratchImage> timage(new (std::nothrow) DirectX::ScratchImage);
    if (!timage)
    {
        PLOG_ERROR << "Memory allocation failed";
        return 1;
    }

    HRESULT hr = Decompress(img, nimg, info, DXGI_FORMAT_UNKNOWN /* picks good default */, *timage);
    if (FAILED(hr))
    {
        PLOG_ERROR << "Failed to decompress: " << name;
        return 1;
    }

    auto& tinfo = timage->GetMetadata();
    info.format = tinfo.format;
    return 0;
}

int TexturesOptimizer::resize(size_t targetWidth, size_t targetHeight)
{

    if (info.width != targetWidth || info.height != targetHeight)
    {
        fitPowerOfTwo(targetWidth, targetHeight);

        std::unique_ptr<DirectX::ScratchImage> timage(new (std::nothrow) DirectX::ScratchImage);
        if (!timage)
        {
            PLOG_ERROR << "Memory allocation failed";
            return 1;
        }

        HRESULT hr = Resize(image->GetImages(), image->GetImageCount(), image->GetMetadata(), targetWidth, targetHeight, DirectX::TEX_FILTER_DEFAULT, *timage);
        if (FAILED(hr))
        {
            PLOG_ERROR << "Failed to resize: " + name;
            return 1;
        }

        auto& tinfo = timage->GetMetadata();

        assert(tinfo.width == twidth && tinfo.height == theight && tinfo.mipLevels == 1);
        info.width = tinfo.width;
        info.height = tinfo.height;
        info.mipLevels = 1;

        image.swap(timage);
    }
}

int TexturesOptimizer::generateMipMaps()
{
    DWORD dwFilter3D = DirectX::TEX_FILTER_DEFAULT;
    size_t tMips;
    if ((!tMips || info.mipLevels != tMips) && (info.mipLevels != 1))
    {
        // Mips generation only works on a single base image, so strip off existing mip levels
        // Also required for preserve alpha coverage so that existing mips are regenerated

        std::unique_ptr<DirectX::ScratchImage> timage(new (std::nothrow) DirectX::ScratchImage);
        if (!timage)
        {
            PLOG_ERROR << "Memory allocation failed";
            return 1;
        }

        DirectX::TexMetadata mdata = info;
        mdata.mipLevels = 1;
        HRESULT hr = timage->Initialize(mdata);
        if (FAILED(hr))
        {
            PLOG_ERROR << "Failed to copy texture data to single level (when generating mipmaps):" + name;
            return 1;
        }

        if (info.dimension == DirectX::TEX_DIMENSION_TEXTURE3D)
        {
            for (size_t d = 0; d < info.depth; ++d)
            {
                hr = CopyRectangle(*image->GetImage(0, 0, d), DirectX::Rect(0, 0, info.width, info.height),
                                   *timage->GetImage(0, 0, d), DirectX::TEX_FILTER_DEFAULT, 0, 0);
                if (FAILED(hr))
                {
                    PLOG_ERROR << "Failed to copy texture data to single level (when generating mipmaps):" + name;
                    return 1;
                }
            }
        }
        else
        {
            for (size_t i = 0; i < info.arraySize; ++i)
            {
                hr = CopyRectangle(*image->GetImage(0, i, 0), DirectX::Rect(0, 0, info.width, info.height),
                                   *timage->GetImage(0, i, 0), DirectX::TEX_FILTER_DEFAULT, 0, 0);
                if (FAILED(hr))
                {
                    PLOG_ERROR << "Failed to copy texture data to single level (when generating mipmaps):" + name;
                    return 1;
                }
            }
        }

        image.swap(timage);
        info.mipLevels = image->GetMetadata().mipLevels;
    }

    if ((!tMips || info.mipLevels != tMips) && (info.width > 1 || info.height > 1 || info.depth > 1))
    {
        std::unique_ptr<DirectX::ScratchImage> timage(new (std::nothrow) DirectX::ScratchImage);
        if (!timage)
        {
            PLOG_ERROR << "Memory allocation failed";
            return 1;
        }

        HRESULT hr;
        if (info.dimension == DirectX::TEX_DIMENSION_TEXTURE3D)
            hr = GenerateMipMaps3D(image->GetImages(), image->GetImageCount(), image->GetMetadata(), dwFilter3D, tMips, *timage);
        else
            hr = GenerateMipMaps(image->GetImages(), image->GetImageCount(), image->GetMetadata(), DirectX::TEX_FILTER_DEFAULT, tMips, *timage);
        if (FAILED(hr))
        {
            PLOG_ERROR << "Failed to generate mipmaps";
            return 1;
        }

        auto& tinfo = timage->GetMetadata();
        info.mipLevels = tinfo.mipLevels;

        image.swap(timage);
    }
}

bool TexturesOptimizer::convertIncompatibleTextures() //Convert TGA textures to DDS
{/*
    if(isIncompatible(filePath))
    {
        PLOG_INFO << tr("Incompatible texture found: ")
                     + filePath.mid(filePath.lastIndexOf("/")) + "\n" + tr("Compressing...");

        convert(CAO_TEXTURES_FORMAT);

        return true;
    }
    else
    */    return false;
}


bool TexturesOptimizer::isCompressed()
{
    return DirectX::IsCompressed(info.format);
}

DirectX::TexMetadata TexturesOptimizer::getInfo()
{
    return info;
}

int TexturesOptimizer::convert(const DXGI_FORMAT& format)
{
    if(!image)
    {
        PLOG_ERROR << "Texture cannot be loaded: " << name;
        return 1;
    }

    if (info.format != format && !DirectX::IsCompressed(format))
    {
        std::unique_ptr<DirectX::ScratchImage> timage(new (std::nothrow) DirectX::ScratchImage);
        if (!timage)
        {
            PLOG_ERROR << "Memory allocation failed";
            return 1;
        }

        HRESULT hr = Convert(image->GetImages(), image->GetImageCount(), image->GetMetadata(), format,
                             0, DirectX::TEX_THRESHOLD_DEFAULT, *timage);
        if (FAILED(hr))
        {
            wprintf(L" FAILED [convert] (%x)\n", hr);
            return 1;
        }

        auto& tinfo = timage->GetMetadata();

        assert(tinfo.format == tformat);
        info.format = tinfo.format;

        image.swap(timage);
    }
}

HRESULT TexturesOptimizer::saveToFile(const QString &filePath)
{
    auto img = image->GetImage(0, 0, 0);
    assert(img);
    size_t nimg = image->GetImageCount();

    // Write texture
    wchar_t wFilePath[1024];
    filePath.toWCharArray(wFilePath);
    HRESULT hr = SaveToDDSFile(img, nimg, info, DirectX::DDS_FLAGS_NONE, wFilePath);

    if (FAILED(hr))
        wprintf(L" FAILED (%x)\n", hr);
}

bool TexturesOptimizer::isIncompatible()
{
    bool isIncompatible = false;

    //TODO replace the cao extensions list with a tga checkbox
    //TODO interface textures cannot be compressed for some games

    //Checking incompatibility with file format
    DXGI_FORMAT fileFormat = info.format;
    for(const auto& format : CAO_TEXTURES_INCOMPATIBLE_FORMATS)
        if(fileFormat == format)
        {
            isIncompatible = true;
            break;
        }

    return isIncompatible;
}

void TexturesOptimizer::fitPowerOfTwo(uint& resultX, uint& resultY)
{
    //Finding nearest power of two
    resultX = info.width;
    if(resultX & 1)
    {
        resultX--;
        resultX |= resultX >> 1;
        resultX |= resultX >> 2;
        resultX |= resultX >> 4;
        resultX |= resultX >> 8;
        resultX |= resultX >> 16;
        resultX++;
    }

    resultY = info.height;
    if(resultY & 1)
    {
        resultY--;
        resultY |= resultY >> 1;
        resultY |= resultY >> 2;
        resultY |= resultY >> 4;
        resultY |= resultY >> 8;
        resultY |= resultY >> 16;
        resultY++;
    }

    if(resultX > resultY)
        resultX = resultY;
    else if(resultY > resultX)
        resultY = resultX;
}
