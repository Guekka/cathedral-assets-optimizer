/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"

namespace CAO {
class TextureCompressionDevice
{
public:
    TextureCompressionDevice(int adapterIndex);

    explicit operator bool() const;
    bool isValid() const;

    ID3D11Device *getDevice() const;

    QString gpuName() const;

private:
    Microsoft::WRL::ComPtr<ID3D11Device> _pDevice;
    QString gpuName_;

    bool createDevice(uint adapter, ID3D11Device **pDevice);
    static bool getDXGIFactory(IDXGIFactory1 **pFactory);
};
} // namespace CAO
