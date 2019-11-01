/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "TextureDecompress.hpp"

namespace CAO {
int TextureDecompress::process(File &file, const OptionsCAO &options)
{
    auto texFile = dynamic_cast<TextureFile *>(&file);
    if (!texFile)
        return 3;

    const auto &image = texFile->getFile();
    const auto img = image.GetImages();
    const size_t nimg = image.GetImageCount();
    auto info = image.GetMetadata();

    auto timage = std::make_unique<DirectX::ScratchImage>();
    const auto hr = Decompress(img, nimg, info, DXGI_FORMAT_UNKNOWN /* picks good default */, *timage);
    if (FAILED(hr))
        return 1;

    //This file is "unmodified". Decompressing the file is only done in order to perform other operations.
    texFile->setFileUnmodified(timage);
    return 0;
}

bool TextureDecompress::isApplicable(File &file, const OptionsCAO &options)
{
    auto texFile = dynamic_cast<TextureFile *>(&file);
    if (!texFile)
        return false;

    return DirectX::IsCompressed(texFile->getFile().GetMetadata().format);
}
} // namespace CAO
