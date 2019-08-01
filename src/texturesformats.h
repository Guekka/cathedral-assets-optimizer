/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once
#include "pch.h"

//Used to convert enum to string and vice versa

namespace detail
{
#define DEFFMT(fmt) \
    { \
#fmt, DXGI_FORMAT_##fmt \
    }
struct SValue
{
    std::string name;
    DXGI_FORMAT format;
};

const SValue DxgiFormats[] = {

    // List does not include _TYPELESS or depth/stencil formats
    DEFFMT(R32G32B32A32_FLOAT),
    DEFFMT(R32G32B32A32_UINT),
    DEFFMT(R32G32B32A32_SINT),
    DEFFMT(R32G32B32_FLOAT),
    DEFFMT(R32G32B32_UINT),
    DEFFMT(R32G32B32_SINT),
    DEFFMT(R16G16B16A16_FLOAT),
    DEFFMT(R16G16B16A16_UNORM),
    DEFFMT(R16G16B16A16_UINT),
    DEFFMT(R16G16B16A16_SNORM),
    DEFFMT(R16G16B16A16_SINT),
    DEFFMT(R32G32_FLOAT),
    DEFFMT(R32G32_UINT),
    DEFFMT(R32G32_SINT),
    DEFFMT(R10G10B10A2_UNORM),
    DEFFMT(R10G10B10A2_UINT),
    DEFFMT(R11G11B10_FLOAT),
    DEFFMT(R8G8B8A8_UNORM),
    DEFFMT(R8G8B8A8_UNORM_SRGB),
    DEFFMT(R8G8B8A8_UINT),
    DEFFMT(R8G8B8A8_SNORM),
    DEFFMT(R8G8B8A8_SINT),
    DEFFMT(R16G16_FLOAT),
    DEFFMT(R16G16_UNORM),
    DEFFMT(R16G16_UINT),
    DEFFMT(R16G16_SNORM),
    DEFFMT(R16G16_SINT),
    DEFFMT(R32_FLOAT),
    DEFFMT(R32_UINT),
    DEFFMT(R32_SINT),
    DEFFMT(R8G8_UNORM),
    DEFFMT(R8G8_UINT),
    DEFFMT(R8G8_SNORM),
    DEFFMT(R8G8_SINT),
    DEFFMT(R16_FLOAT),
    DEFFMT(R16_UNORM),
    DEFFMT(R16_UINT),
    DEFFMT(R16_SNORM),
    DEFFMT(R16_SINT),
    DEFFMT(R8_UNORM),
    DEFFMT(R8_UINT),
    DEFFMT(R8_SNORM),
    DEFFMT(R8_SINT),
    DEFFMT(A8_UNORM),
    DEFFMT(R9G9B9E5_SHAREDEXP),
    DEFFMT(R8G8_B8G8_UNORM),
    DEFFMT(G8R8_G8B8_UNORM),
    DEFFMT(BC1_UNORM),
    DEFFMT(BC1_UNORM_SRGB),
    DEFFMT(BC2_UNORM),
    DEFFMT(BC2_UNORM_SRGB),
    DEFFMT(BC3_UNORM),
    DEFFMT(BC3_UNORM_SRGB),
    DEFFMT(BC4_UNORM),
    DEFFMT(BC4_SNORM),
    DEFFMT(BC5_UNORM),
    DEFFMT(BC5_SNORM),
    DEFFMT(B5G6R5_UNORM),
    DEFFMT(B5G5R5A1_UNORM),

    // DXGI 1.1 formats
    DEFFMT(B8G8R8A8_UNORM),
    DEFFMT(B8G8R8X8_UNORM),
    DEFFMT(R10G10B10_XR_BIAS_A2_UNORM),
    DEFFMT(B8G8R8A8_UNORM_SRGB),
    DEFFMT(B8G8R8X8_UNORM_SRGB),
    DEFFMT(BC6H_UF16),
    DEFFMT(BC6H_SF16),
    DEFFMT(BC7_UNORM),
    DEFFMT(BC7_UNORM_SRGB),

    // DXGI 1.2 formats
    DEFFMT(AYUV),
    DEFFMT(Y410),
    DEFFMT(Y416),
    DEFFMT(YUY2),
    DEFFMT(Y210),
    DEFFMT(Y216),
    // No support for legacy paletted video formats (AI44, IA44, P8, A8P8)
    DEFFMT(B4G4R4A4_UNORM),
};
#undef DEFMTT //cleanup
} // namespace detail

inline DXGI_FORMAT stringToDxgiFormat(const std::string &string)
{
    for (const auto &format : detail::DxgiFormats)
    {
        if (format.name == string)
            return format.format;
    }
    return DXGI_FORMAT_UNKNOWN;
}

inline std::string dxgiFormatToString(const DXGI_FORMAT &format)
{
    for (const auto &dxFormat : detail::DxgiFormats)
    {
        if (dxFormat.format == format)
            return dxFormat.name;
    }
    return "DXGI_FORMAT_UNKNOWN";
}
