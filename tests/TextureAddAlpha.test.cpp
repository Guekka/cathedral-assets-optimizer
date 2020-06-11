/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Commands/Textures/TextureAddAlpha.hpp"
#include "utils.hpp"

using namespace CAO;

TEST_CASE("Detecting if TextureAddAlpha is applicable by settings")
{
    PatternSettings sets;
    sets.bTexturesLandscapeAlpha = true;
    auto file                    = getStandardTextureFile(sets);

    TextureAddAlpha command;

    currentProfile().getFileTypes().slTextureLandscapes = {};
    CHECK_FALSE(command.isApplicable(*file));
}
/*
 * This was an attempt at testing the alpha detection. But it is quite dumb, as it uses the same function
 * that TextureAddAlpha::process. So if the test succeeded, it wouldn't mean the function is working
 
TEST_CASE("r")
{

    PatternSettings sets;
    sets.bTexturesLandscapeAlpha = true;
    auto file                    = getStandardTextureFile(sets);

    auto texFile     = dynamic_cast<const TextureResource *>(&file->getFile());

    auto transform = [](DirectX::XMVECTOR *outPixels,
                        const DirectX::XMVECTOR *inPixels,
                        size_t width,
                        [[maybe_unused]] size_t y) {
        const auto black = DirectX::XMVectorSet(0, 0, 0, 1);
        for (size_t j = 0; j < width; ++j)
            outPixels[j] = XMVectorSelect(black, inPixels[j], DirectX::g_XMSelect1110);
    };

    auto timage = std::make_unique<TextureResource>();

    auto hr = DirectX::TransformImage(texFile->GetImages(),
                                      texFile->GetImageCount(),
                                      texFile->GetMetadata(),
                                      transform,
                                      *timage2);


}
*/
