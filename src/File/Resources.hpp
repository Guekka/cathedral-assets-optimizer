/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <optional>
#include <variant>

#include <QObject>

#include "Commands/Textures/TextureFormats.hpp"
#include <DirectXTex.h>
#include <NifFile.hpp>

#include "libbsarch/src/bsa_saver.hpp"
#include "libbsarch/src/transform_archive.hpp"

namespace CAO::Resources {
class Mesh : public nifly::NifFile
{
};
class Texture : public DirectX::ScratchImage
{
public:
    DXGI_FORMAT origFormat{};
};

struct BSAFile
{
    BSAFile()
        : saver(bsa)
    {
    }

    ~BSAFile() = default;

    BSAFile(const BSAFile &) = delete;

    BSAFile(BSAFile &&other) noexcept
        : bsa(std::move(other.bsa))
        , saver(bsa)
    {
        saver = std::move(other.saver);
        saver.set_bsa(bsa);
    }

    BSAFile &operator=(const BSAFile &) = delete;
    BSAFile &operator                   =(BSAFile &&other)
    {
        bsa   = std::move(other.bsa);
        saver = std::move(other.saver);
        saver.set_bsa(bsa);
        return *this;
    }

    libbsarch::bsa bsa;
    libbsarch::bsa_saver_simple saver;
    std::optional<libbsarch::transform_callback> callback;
};

struct BSAFolder
{
    std::vector<BSAFile> bsas;
};

struct Animation
{
};
} // namespace CAO::Resources

namespace CAO {
using Resource = std::variant<std::monostate,
                              Resources::Mesh,
                              Resources::Texture,
                              Resources::BSAFile,
                              Resources::BSAFolder,
                              Resources::Animation>;

} // namespace CAO

Q_DECLARE_METATYPE(nifly::NiFileVersion)
Q_DECLARE_METATYPE(bsa_archive_type_e)
