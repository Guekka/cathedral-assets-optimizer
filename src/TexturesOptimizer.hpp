/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "FilesystemOperations.hpp"
#include "PluginsOperations.hpp"
#include "Profiles.hpp"
#include "pch.hpp"

class TexturesOptimizer final : public QObject
{
public:
    TexturesOptimizer();

    void listLandscapeTextures(QDirIterator &it);

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
    bool isCompressed() const;
    [[nodiscard]] bool canBeCompressed() const;
    /*!
   * \brief Perform various optimizations on the current texture
   * \return False if an error happens
   */
    bool optimize(const bool &bNecessary,
                  const bool &bCompress,
                  const bool &bMipmaps,
                  const std::optional<size_t> &tWidth,
                  const std::optional<size_t> &tHeight);

    void dryOptimize(const bool &bNecessary,
                     const bool &bCompress,
                     const bool &bMipmaps,
                     const std::optional<size_t> &tWidth,
                     const std::optional<size_t> &tHeight);

    bool resize(size_t targetWidth, size_t targetHeight);

    static void fitPowerOfTwo(uint &resultX, uint &resultY);
    bool isPowerOfTwo() const;

    bool generateMipMaps();
    bool canHaveMipMaps();
    size_t calculateOptimalMipMapsNumber() const;

    bool convertLandscapeTextures();

    DirectX::TexMetadata getInfo() const;

    bool isIncompatible() const;

    static bool compareInfo(const DirectX::TexMetadata &info1, const DirectX::TexMetadata &info2);

    struct TexOptOptionsResult
    {
        bool bNeedsResize;
        bool bNeedsCompress;
        bool bNeedsMipmaps;
        size_t tWidth;
        size_t tHeight;
    };

    TexOptOptionsResult processArguments(const bool &bNecessary,
                                         const bool &bCompress,
                                         const bool &bMipmaps,
                                         const std::optional<size_t> &tWidth,
                                         const std::optional<size_t> &tHeight);

    bool modifiedCurrentTexture = false;

private:
    QStringList _landscapeTextures;
};
