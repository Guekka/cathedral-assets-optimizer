/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <QDirIterator>

#include "Commands/Plugins/PluginsOperations.hpp"
#include "Commands/Textures/TextureConvert.hpp"
#include "File/Textures/TextureFile.hpp"
#include "TextureAddAlpha.hpp"
#include "Utils/Algorithms.hpp"
#include "Utils/Filesystem.hpp"

namespace CAO {

TextureAddAlpha::TextureAddAlpha()
{
    Profiles::callWhenRunStart([] {
        listLandscapeTextures(currentProfile().getGeneralSettings(), currentProfile().getFileTypes());
    });
}

CommandResult TextureAddAlpha::process(File &file) const
{
    const auto *texFile = file.getFile<Resources::Texture>();
    if (!texFile)
        return CommandResultFactory::getCannotCastFileResult();

    Resources::Texture timage;

    if (!DirectX::HasAlpha(texFile->GetMetadata().format))
    {
        const auto hr = TextureConvert::convertWithoutCompression(*texFile,
                                                                  timage,
                                                                  DXGI_FORMAT_R8G8B8A8_UNORM);
        if (FAILED(hr))
        {
            return CommandResultFactory::getFailedResult(
                hr, "Failed to convert landscape texture to a format with alpha");
        }
    }
    constexpr auto transform = [](DirectX::XMVECTOR *outPixels,
                                  const DirectX::XMVECTOR *inPixels,
                                  size_t width,
                                  [[maybe_unused]] size_t y) {
        const auto black = DirectX::XMVectorSet(0, 0, 0, 0);
        for (size_t j = 0; j < width; ++j)
            outPixels[j] = XMVectorSelect(black, inPixels[j], DirectX::g_XMSelect1110);
    };

    Resources::Texture timage2;

    const auto hr = DirectX::TransformImage(timage.GetImages(),
                                            timage.GetImageCount(),
                                            timage.GetMetadata(),
                                            transform,
                                            timage2);

    if (FAILED(hr))
    {
        return CommandResultFactory::getFailedResult(hr, "Failed to add alpha to landscape textures.");
    }

    file.setFile(std::move(timage2));
    return CommandResultFactory::getSuccessfulResult();
}

CommandState TextureAddAlpha::isApplicable(File &file) const
{
    if (!file.patternSettings().bTexturesLandscapeAlpha())
        return CommandState::NotRequired;

    if (!isLandscape(file.getInputFilePath()))
        return CommandState::NotRequired;

    const auto *texFile = file.getFile<Resources::Texture>();
    if (!texFile)
        return CommandState::NotRequired;

    if (DirectX::IsCompressed(texFile->GetMetadata().format))
        return CommandState::PendingPreviousSteps;

    if (!DirectX::HasAlpha(texFile->GetMetadata().format))
        return CommandState::Ready;

    if (texFile->GetMetadata().GetAlphaMode() == DirectX::TEX_ALPHA_MODE_OPAQUE)
        return CommandState::Ready;

    if (!texFile->IsAlphaAllOpaque())
        return CommandState::NotRequired;

    return CommandState::Ready;
}

bool TextureAddAlpha::isLandscape(const QString &filepath)
{
    const auto &ft = currentProfile().getFileTypes();
    return ft.match(ft.slTextureLandscapes(), filepath);
}

void TextureAddAlpha::listLandscapeTextures(const GeneralSettings &settings, FileTypes &filetypes)
{
    const bool severalMods = settings.eMode() == SeveralMods;
    const auto flags       = severalMods ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;

    std::vector<std::string> landTextures = filetypes.slTextureLandscapes();

    QDirIterator it(settings.sInputPath(), flags);
    for (const auto &plugin : Filesystem::listPlugins(it))
    {
        const auto &result = PluginsOperations::listLandscapeTextures(plugin);
        landTextures.insert(landTextures.end(), result.begin(), result.end());
    }

    for (auto &tex : landTextures)
        if (ends_with(std::string_view(tex), std::string_view("_n.dds")))
            tex.insert(tex.size() - 4, "_n");

    remove_duplicates(landTextures);

    filetypes.slTextureLandscapes = landTextures;
}

} // namespace CAO
