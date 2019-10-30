/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"

class TextureCompressionDevice
{
public:
    TextureCompressionDevice();
    virtual ~TextureCompressionDevice();

    explicit operator bool() const;
    bool isValid() const;

    Microsoft::WRL::ComPtr<ID3D11Device> getDevice() const;

protected:
    Microsoft::WRL::ComPtr<ID3D11Device> _pDevice;

private:
    bool createDevice(int adapter, ID3D11Device **pDevice) const;
    bool getDXGIFactory(IDXGIFactory1 **pFactory) const;
};
