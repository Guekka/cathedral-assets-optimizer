/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Commands/Textures/TextureResize.hpp"
#include "utils.hpp"

using namespace CAO;

TEST_CASE_FIXTURE(TextureResize, "Target resizing in ratio way yields good results")
{
    DirectX::TexMetadata info;
    info.width  = 1024;
    info.height = 1024;

    PatternSettings sets;
    sets.eTexturesResizingMode          = ByRatio;
    sets.iTexturesResizingByRatioWidth  = 8;
    sets.iTexturesResizingByRatioHeight = 8;

    auto tinfo = calculateTargetDimensions(info, sets);

    CHECK_EQ(tinfo.width, 128);
    CHECK_EQ(tinfo.height, 128);
}

TEST_CASE_FIXTURE(TextureResize, "Target resizing in fixed size way yields good results")
{
    DirectX::TexMetadata info;
    info.width  = 1024;
    info.height = 1024;

    PatternSettings sets;
    sets.eTexturesResizingMode         = BySize;
    sets.iTexturesResizingBySizeWidth  = 128;
    sets.iTexturesResizingBySizeHeight = 128;

    auto tinfo = calculateTargetDimensions(info, sets);

    CHECK_EQ(tinfo.width, 128);
    CHECK_EQ(tinfo.height, 128);
}
