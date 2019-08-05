/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Games.h"
#include "pch.h"

class TexturesOptimizer : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(TexturesOptimizer)

public:
    TexturesOptimizer();
    enum TextureType
    {
        dds,
        tga
    };
    bool open(const void *pSource, const size_t &size, const TextureType &type, const QString &fileName);
    bool open(const QString &filePath, const TextureType &type);

    bool saveToFile(const QString &filePath);
    /*!
     * \brief Decompress the current texture. It is required to use several functions.
     * \return False if an error happens
     */
    bool decompress();
    /*!
     * \brief Convenience function, that will use appropriately convertWithCompression or convertWithoutCompression
     * \param format The format to use
     * \return False in case of error
     */
    bool convert(const DXGI_FORMAT &format);
    /*!
     * \brief Compress the file using the provided compression format,
     * \param format The format to use
     */
    bool convertWithCompression(const DXGI_FORMAT &format);
    bool convertWithoutCompression(const DXGI_FORMAT &format);
    /*!
     * \brief Check if a texture is compressed
     * \return True if the file is compressed
     */
    bool isCompressed();
    bool canBeCompressed();
    /*!
     * \brief Perform various optimizations on the current texture
     * \param optLevel The optimization level
     * \return False if an error happens
     */
    bool optimize(const bool &bNecessary,
                  const bool &bCompress,
                  const bool &bMipmaps,
                  const std::optional<size_t> &twidth,
                  const std::optional<size_t> &theight);

    void dryOptimize(const bool &bNecessary,
                     const bool &bCompress,
                     const bool &bMipmaps,
                     const std::optional<size_t> &twidth,
                     const std::optional<size_t> &theight);

    bool resize(size_t targetWidth, size_t targetHeight);

    static void fitPowerOfTwo(uint &resultX, uint &resultY);
    bool isPowerOfTwo();

    bool generateMipMaps();
    bool canHaveMipMaps();
    size_t calculateOptimalMipMapsNumber();

    DirectX::TexMetadata getInfo();

    bool isIncompatible();

    static bool compareInfo(const DirectX::TexMetadata &info1, const DirectX::TexMetadata &info2);

private:
    std::unique_ptr<DirectX::ScratchImage> image{};
    DirectX::TexMetadata info{};
    QString name;
    TextureType _type;

    Microsoft::WRL::ComPtr<ID3D11Device> pDevice;

    bool createDevice(int adapter, ID3D11Device **pDevice);
    bool GetDXGIFactory(IDXGIFactory1 **pFactory);
};
