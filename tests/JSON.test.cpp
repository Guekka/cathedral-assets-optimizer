/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https,//mozilla.org/MPL/2.0/. */

#include "settings/json.hpp"

#include "doctest/doctest.h"

using namespace cao;

TEST_CASE("Removing duplicates between several JSONs")
{
    // clang-format off
    nlohmann::json master = {
        {"Advanced",
        {
            {"Animations", {
                {"eAnimationsFormat", 3}
            }},
            {"BSA",
            {
                {"BSAbBSAIsTexture", false},
                {"bBSAIsStandard", false},
                {"bBSAIsUncompressible", false},
                {"bBSATexturesEnabled", true},
                {"eBSAFormat", 4},
                {"eBSATexturesFormat", 4},
                {"bsa_max_size", 2100000000},
                {"iBSATexturesMaxSize", 2544768122.88}
            }},
            {"Meshes",
            {
                {"eMeshesFileVersion", 335675399},
                {"iMeshesStream", 100},
                {"iMeshesUser", 12}
            }},
            {"Textures",
            {
                {"bTexturesForceConvert", false},
                {"eTexturesFormat", 98},
                {"slTextureUnwantedFormats", {85, 86, 115}}
            }}
        }}
    };

    //One different line
    nlohmann::json input1 = {
        {"Advanced",
        {
            {"Animations", {
                {"eAnimationsFormat", 4} //This line is different
            }},
            {"Meshes",
            {
                {"eMeshesFileVersion", 335675399},
                {"iMeshesStream", 100},
                {"iMeshesUser", 12}
            }}
        }}
        };

    nlohmann::json expected1 = {
        {"Advanced",
        {
            {"Animations", {
                {"eAnimationsFormat", 4}
            }}
        }}
    };

    //Entirely included in master
    nlohmann::json input2 = {
        {"Advanced",
        {
            {"Animations", {
                {"eAnimationsFormat",3} 
            }},
            {"Meshes",
            {
                {"eMeshesFileVersion", 335675399},
                {"iMeshesStream", 100},
                {"iMeshesUser", 12}
            }}
        }}
        };

    //No expected, since it should be deleted

    // clang-format on
    std::vector input{input1, input2};
    std::vector expected{expected1};

    JSON::removeDuplicates(master, input);

    CHECK_EQ(input, expected);
}
