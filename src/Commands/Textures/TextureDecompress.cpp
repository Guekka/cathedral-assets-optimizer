/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "TextureDecompress.hpp"
#include "File/Textures/TextureFile.hpp"

namespace CAO {
CommandResult TextureDecompress::process(File &file) const
{
    auto texFile = dynamic_cast<const TextureResource *>(&file.getFile());
    if (!texFile)
        return CommandResultFactory::getCannotCastFileResult();

    const auto &img    = texFile->GetImages();
    const size_t &nimg = texFile->GetImageCount();
    const auto &info   = texFile->GetMetadata();

    auto timage   = std::make_unique<TextureResource>();
    const auto hr = Decompress(img, nimg, info, DXGI_FORMAT_UNKNOWN /* picks good default */, *timage);
    if (FAILED(hr))
        return CommandResultFactory::getFailedResult(hr, "Failed to decompress");

    //This file is "unmodified". Decompressing the file is only done in order to perform other operations.
    file.setFile(std::unique_ptr<Resource>(std::move(timage)), false);
    return CommandResultFactory::getSuccessfulResult();
}

CommandState TextureDecompress::isApplicable(File &file) const
{
    auto texFile = dynamic_cast<const TextureResource *>(&file.getFile());
    if (!texFile)
        return CommandState::NotRequired;

    if (!DirectX::IsCompressed(texFile->GetMetadata().format))
        return CommandState::NotRequired;

    return CommandState::Ready;
}
} // namespace CAO
