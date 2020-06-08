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
        CHECK(patterns.find(0) != patterns.end());
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

        const std::map<size_t, PatternSettings> expected{{0, sets1}, {1, sets2}, {2, sets3}};
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

    auto const expected = std::map<size_t, PatternSettings>{{0, expectedSets1}, {1, expectedSets3}};

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

TEST_CASE("Free slot")
{
    SUBCASE("First")
    {
        const std::map<size_t, PatternSettings> input{{0, {}}, {1, {}}, {2, {}}};
        const std::map<size_t, PatternSettings> expected{{1, {}}, {2, {}}, {3, {}}};

        PatternMap map;
        map.get() = input;
        map.freeSlot(0);

        CHECK(map.get() == expected);
    }
    SUBCASE("Second")
    {
        const std::map<size_t, PatternSettings> input{{0, {}}, {2, {}}, {4, {}}};
        const std::map<size_t, PatternSettings> expected{{1, {}}, {3, {}}, {5, {}}};

        PatternMap map;
        map.get() = input;
        map.freeSlot(0);

        CHECK(map.get() == expected);
    }

    SUBCASE("Third")
    {
        const std::map<size_t, PatternSettings> input{{0, {}}, {2, {}}, {4, {}}};
        const std::map<size_t, PatternSettings> expected{{0, {}}, {3, {}}, {5, {}}};

        PatternMap map;
        map.get() = input;
        map.freeSlot(2);

        CHECK(map.get() == expected);
    }
}
