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
    bool isTGA = false;
    DXGI_FORMAT origFormat{};
};

class BSAFolderResource : public Resource, public QDir
{};

class BSAFileResource : public Resource, public libbsarch::bs_archive_auto
{
};

struct AnimationResource : public Resource
{
    hkVariant root;
    hkResource *resource;
    hkPackFormat pkFormat;
};

} // namespace CAO
