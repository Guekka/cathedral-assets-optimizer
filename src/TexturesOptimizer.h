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
    static bool convertIncompatibleTextures(const QString& filePath);
    /*!
     * \brief Will compress a DDS file using the appropriate compression format, only if the texture is uncompressed and if it has more than 16 pixels
     * \param filePath The file to convert
     */
    static void compress(const QString& filePath);
    /*!
     * \brief Will check if a texture is compressed
     * \param filePath The file to check
     * \return A bool : true if the file is compressed, false otherwise.
     */
    static bool isCompressed(const QString &filePath);

    static QString getFormat(const QString &filePath);

    static void convertTexture(const QString& filePath, const QString& format);

    static bool isIncompatible(const QString &filePath);

    enum TextureType {dds, tga};

    HRESULT open(const void* pSource, size_t size, TextureType type);
    HRESULT open(QString& filePath, TextureType type);

private:
    std::unique_ptr<DirectX::ScratchImage> image;
    DirectX::TexMetadata info;


};
