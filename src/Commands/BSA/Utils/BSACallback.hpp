/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "File/Resources.hpp"
#include "File/Textures/TextureFile.hpp"

namespace CAO {
inline void BSACallback([[maybe_unused]] bsa_archive_t archive,
                        const wchar_t *file_path,
                        bsa_dds_info_t *dds_info,
                        void *context)
{
    const QString &path = *static_cast<QString *>(context) + '/' + QString::fromWCharArray(file_path);

    TextureFile tex;
    if (auto result = tex.loadFromDisk(path))
    {
        PLOG_ERROR << QString("Failed to open DDS '%1' when packing archive: ").arg(path);
    }

    const auto &info = tex.getFile<Resources::Texture>()->GetMetadata();

    dds_info->width   = info.width;
    dds_info->height  = info.height;
    dds_info->mipmaps = info.mipLevels;
}
} // namespace CAO
