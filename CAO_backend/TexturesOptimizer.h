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
     * \brief Will convert a TGA file to DDS. If a DDS with the same name already exist, the TGA is removed.
     * \param filePath The file to convert
     */
    static void convertTgaToDds(const QString& filePath);
    /*!
     * \brief Will compress a DDS file using BC7 compression if the texture is uncompressed
     * \param filePath The file to convert
     */
    static void convertToBc7IfUncompressed(const QString& filePath);
    /*!
     * \brief Will check if a texture is compressed
     * \param filePath The file to check
     * \return A bool : true if the file is compressed, false otherwise.
     */
    static bool isCompressed(const QString &filePath);

};
