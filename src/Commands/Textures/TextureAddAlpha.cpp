/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "TextureAddAlpha.hpp"
#include "Commands/Plugins/PluginsOperations.hpp"
#include "Commands/Textures/TextureConvert.hpp"
#include "Utils/Filesystem.hpp"

namespace CAO {

CommandResult TextureAddAlpha::process(File &file)
{
    auto texFile = dynamic_cast<const TextureResource *>(&file.getFile());
    if (!texFile)
        return _resultFactory.getCannotCastFileResult();

    auto timage = std::make_unique<TextureResource>();

    if (!DirectX::HasAlpha(texFile->GetMetadata().format))
        if (TextureConvert::convertWithoutCompression(*texFile, *timage, DXGI_FORMAT_R8G8B8A8_UNORM))
        {
            return _resultFactory
                .getFailedResult(1, "Failed to convert landscape texture to a format with alpha");
        }

    auto transform = [](DirectX::XMVECTOR *outPixels,
                        const DirectX::XMVECTOR *inPixels,
                        size_t width,
                        [[maybe_unused]] size_t y) {
        const auto black = DirectX::XMVectorSet(0, 0, 0, 0);
        for (size_t j = 0; j < width; ++j)
            outPixels[j] = XMVectorSelect(black, inPixels[j], DirectX::g_XMSelect1110);
    };

    auto timage2 = std::make_unique<TextureResource>();

    auto hr = DirectX::TransformImage(timage->GetImages(),
                                      timage->GetImageCount(),
                                      timage->GetMetadata(),
                                      transform,
                                      *timage2);

    if (FAILED(hr))
    {
        return _resultFactory
            .getFailedResult(1, QString("Failed to add alpha to landscape textures. HR: %1").arg(hr));
    }

    file.setFile(std::unique_ptr<Resource>(std::move(timage2)));
    return _resultFactory.getSuccessfulResult();
}

bool TextureAddAlpha::isApplicable(File &file)
{
    auto texFile = dynamic_cast<const TextureResource *>(&file.getFile());
    if (!texFile)
        return false;

    if (!file.patternSettings().bTexturesLandscapeAlpha())
        return false;

    if (!isLandscape(file.getName()))
        return false;

    if (!DirectX::HasAlpha(texFile->GetMetadata().format))
        return true;

    bool isFullWhite = true;

    DirectX::EvaluateImage(texFile->GetImages(),
                           texFile->GetImageCount(),
                           texFile->GetMetadata(),
                           [&isFullWhite](const DirectX::XMVECTOR *pixels, size_t width, size_t y) {
                               if (!isFullWhite)
                                   return;

                               for (size_t j = 0; j < width; ++j)
                               {
                                   if (DirectX::XMVectorGetByIndex(pixels[j], 3) != 1)
                                   {
                                       isFullWhite = false;
                                       return;
                                   }
                               }
                           });

    return isFullWhite;
}

bool TextureAddAlpha::isLandscape(const QString &filepath) const
{
    const auto &ft = currentProfile().getFileTypes();
    return ft.match(ft.slTextureLandscapes(), filepath);
}

void TextureAddAlpha::listLandscapeTextures(const GeneralSettings &settings, FileTypes &filetypes)
{
    const bool severalMods = settings.eMode() == SeveralMods;
    const auto flags       = severalMods ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;

    std::vector<std::string> landTextures = filetypes.slTextureLandscapes();

    QDirIterator it(settings.sUserPath(), flags);
    for (const auto &plugin : Filesystem::listPlugins(it))
    {
        const auto &result = PluginsOperations::listLandscapeTextures(plugin);
        landTextures.insert(landTextures.end(), result.begin(), result.end());
    }

    for (auto &tex : landTextures)
        if (endsWith(std::string_view(tex), std::string_view("_n.dds")))
            tex.insert(tex.size() - 4, "_n");

    remove_duplicates(landTextures);

    filetypes.slTextureLandscapes = landTextures;
}

} // namespace CAO
