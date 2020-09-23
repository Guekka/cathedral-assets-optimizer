/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <optional>
#include <QObject>

#include "Commands/Textures/TextureFormats.hpp"
#include "DirectXTex/DirectXTex.h"
#include "NIF/NifFile.h"

#include "hkxcmd/hkPackFormat.hpp"
#include <Common/Base/Types/hkBaseTypes.h>
#include <Common/Base/hkBase.h>
#include <Common/Serialize/Resource/hkResource.h>

#include "libbsarch/src/bsa_saver.hpp"
#include "libbsarch/src/transform_archive.hpp"

namespace CAO {
class Resource
{
public:
    virtual ~Resource() = default;
};

class MeshResource : public Resource, public NifFile
{
};

class TextureResource : public Resource, public DirectX::ScratchImage
{
public:
    DXGI_FORMAT origFormat{};
};

struct BSAFileResource : public Resource
{
    BSAFileResource()
        : saver(bsa)
    {
    }

    ~BSAFileResource() = default;

    BSAFileResource(const BSAFileResource &) = delete;

    BSAFileResource(BSAFileResource &&other) noexcept
        : bsa(std::move(other.bsa))
        , saver(bsa)
    {
        saver = std::move(other.saver);
        saver.set_bsa(bsa);
    }

    BSAFileResource &operator=(const BSAFileResource &) = delete;
    BSAFileResource &operator                           =(BSAFileResource &&other)
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

struct BSAFolderResource : public Resource
{
    std::vector<BSAFileResource> bsas;
};

struct AnimationResource : public Resource
{
    hkVariant root;
    hkResource *resource;
    hkPackFormat pkFormat;
};

} // namespace CAO

Q_DECLARE_METATYPE(NiFileVersion)
Q_DECLARE_METATYPE(bsa_archive_type_e)
