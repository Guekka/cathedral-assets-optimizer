/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Settings/PatternMap.hpp"
#include "utils.hpp"

using namespace CAO;

void printDiff(const nlohmann::json &one, const nlohmann::json &two)
{
    MESSAGE(nlohmann::json::diff(one, two).dump(4));
}

void printDiff(const PatternSettings &one, const PatternSettings &two)
{
    MESSAGE(nlohmann::json::diff(one.getJSON(), two.getJSON()).dump(4));
}

TEST_CASE("Listing patterns")
{
    PatternMap map;
    SUBCASE("Empty json is handled")
    {
        nlohmann::json j;
        map.listPatterns(j);
        auto patterns = map.get();
        CHECK_EQ(patterns.size(), 1);
        CHECK_EQ(patterns[0], PatternSettings{0, {"*"}});
    }
    SUBCASE("Standard json")
    {
        const auto input = R"(
		[
		  {
			"Meshes": {
			  "bMeshesResave": true,
              "iMeshesOptimizationLevel": 3
			},
			"Priority": 0,
			"Patterns": [
			  "*"
			]
		  },
		  {
			"Advanced": {
			  "Textures": {
				"eTexturesFormat": 65
			  }
			},
			"Priority": 1,
			"Patterns": [
			  "*.dds"
			]
		  },
		  {
			"Textures": {
			  "bTexturesMipmaps": false
			},
			"Priority": 2,
			"Patterns": [
			  "*.nif"
			]
		  }
		]
		)"_json;

        PatternSettings sets1(0, {"*"});
        sets1.bMeshesResave = true;
        sets1.iMeshesOptimizationLevel = 3;

        PatternSettings sets2(1, {"*.dds"});
        sets2.eTexturesFormat = DXGI_FORMAT_A8_UNORM;

        PatternSettings sets3(2, {"*.nif"});
        sets3.bTexturesMipmaps = false;

        const std::vector<PatternSettings> expected{sets1, sets2, sets3};
        map.listPatterns(input);

        CHECK_EQ(map.get(), expected);
    }
}

TEST_CASE("cleanPatterns")
{
    PatternSettings sets1(0, {"*"});
    sets1.priority_     = 0;
    sets1.bMeshesResave = true;
    sets1.iMeshesOptimizationLevel = 3;

    PatternSettings sets2 = sets1;
    sets2.priority_       = 3;

    PatternSettings sets3(1, {"*.dds"});
    sets3.eTexturesFormat = DXGI_FORMAT_A8_UNORM;

    PatternSettings expectedSets1 = sets1;
    PatternSettings expectedSets3 = sets3;

    auto const expected = std::vector<PatternSettings>{expectedSets1, expectedSets3};

    PatternMap map;
    map.addPattern(sets1);
    map.addPattern(sets2);
    map.addPattern(sets3);

    map.cleanPatterns();

    CHECK_EQ(map.get(), expected);
}

TEST_CASE("Getting json from PatternMap")
{
    PatternSettings sets1(0, {"*"});
    sets1.priority_     = 0;
    sets1.bMeshesResave = true;
    sets1.iMeshesOptimizationLevel = 3;

    //Should be removed
    PatternSettings sets2 = sets1;
    sets2.patterns_       = {"pattern"};
    sets2.priority_       = 3;

    PatternSettings sets3(1, {"*.dds"});
    sets3.eTexturesFormat = DXGI_FORMAT_A8_UNORM;

    PatternSettings sets4(2, {"*.nif"});
    sets4.bTexturesMipmaps = false;

    const auto expected = R"(
    [
        {
            "Meshes": {
                "bMeshesResave": true,
                "iMeshesOptimizationLevel": 3
            },
            "Patterns": [
                "*",
                "pattern"
            ],
            "Priority": 0
        },
        {
            "Advanced": {
                "Textures": {
                    "eTexturesFormat": 65
                }
            },
            "Patterns": [
                "*.dds"
            ],
            "Priority": 1
        },
        {
            "Patterns": [
                "*.nif"
            ],
            "Priority": 2,
            "Textures": {
                "bTexturesMipmaps": false
            }
        }
    ]
    )"_json;

    PatternMap map;
    map.addPattern(sets1);
    map.addPattern(sets2);
    map.addPattern(sets3);
    map.addPattern(sets4);

    CHECK_EQ(map.getUnifiedJSON(), expected);
}
