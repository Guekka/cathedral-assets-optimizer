/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#include "Games.h"

class TexturesOptimizer : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(TexturesOptimizer)

public:
    /*!
     * \brief Will convert incompatibles textures to DDS. If a DDS with the same name already exist, the incompatible texture is removed.
     * \param filePath The file to convert
     * \return whether true if the file has been converted or false if it hasn't been converted
     */
    bool convertIncompatibleTextures();
    /*!
     * \brief Will compress a DDS file using the appropriate compression format, only if the texture is uncompressed and if it has more than 16 pixels
     * \param filePath The file to convert
     */
    void compress();
    /*!
     * \brief Will check if a texture is compressed
     * \param filePath The file to check
     * \return A bool : true if the file is compressed, false otherwise.
     */
    bool isCompressed();

    DirectX::TexMetadata getInfo();

    void convert(const DXGI_FORMAT& format);

    bool isIncompatible();

    enum TextureType {dds, tga};

    HRESULT open(const void* pSource, const size_t& size, const TextureType& type, const QString& fileName);
    HRESULT open(const QString& filePath, const TextureType& type);

private:
    std::unique_ptr<DirectX::ScratchImage> image;
    DirectX::TexMetadata info;
    QString name;
};
