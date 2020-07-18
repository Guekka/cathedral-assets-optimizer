/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"

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

    BSAFileResource(BSAFileResource &&other)
        : bsa(std::move(other.bsa))
        , saver(bsa)
    {
        saver = std::move(other.saver);
        saver.set_bsa(bsa);
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
