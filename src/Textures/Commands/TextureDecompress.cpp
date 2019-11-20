/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "TextureDecompress.hpp"

namespace CAO {
CommandResult TextureDecompress::process(File &file, const Settings &settings)
{
    auto texFile = dynamic_cast<const TextureResource *>(&file);
    if (!texFile)
        return _resultFactory.getCannotCastFileResult();

    const auto img = texFile->GetImages();
    const size_t nimg = texFile->GetImageCount();
    auto info = texFile->GetMetadata();

    auto timage = std::make_unique<TextureResource>();
    const auto hr = Decompress(img, nimg, info, DXGI_FORMAT_UNKNOWN /* picks good default */, *timage);
    if (FAILED(hr))
        return _resultFactory.getFailedResult(1, "Failed to decompress");

    //This file is "unmodified". Decompressing the file is only done in order to perform other operations.
    file.setFile(*timage.release(), false);
    return _resultFactory.getSuccessfulResult();
}

bool TextureDecompress::isApplicable(File &file, const Settings &settings)
{
    auto texFile = dynamic_cast<const TextureResource *>(&file);
    if (!texFile)
        return false;

    return DirectX::IsCompressed(texFile->GetMetadata().format);
}
} // namespace CAO
