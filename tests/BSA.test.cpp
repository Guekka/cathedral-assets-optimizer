/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Commands/BSA/utils/BSA.hpp"

#include "utils.hpp"

#include <random>

using namespace cao;

//Sorting to allow permutations
void sort(std::vector<BSA> &bsas)
{
    const auto pred = [](auto &&left, auto &&right) {
        if (left.filesSize == right.filesSize)
            return left.type < right.type;
        else
            return left.filesSize < right.filesSize;
    };
    std::sort(bsas.begin(), bsas.end(), pred); //TODO Update to ranges when VS16.8 releases
}

TEST_CASE("Merging BSAs: testing MergeTypes")
{
    std::vector input{BSA{1'000'000, 1'000, StandardBsa},
                      BSA{1'000'000, 1'000, StandardBsa},
                      BSA{1'000'000, 1'000, TexturesBsa},
                      BSA{1'000'000, 1'000, TexturesBsa},
                      BSA{1'000'000, 1'000, UncompressableBsa},
                      BSA{1'000'000, 1'000, UncompressableBsa}};

    std::vector<BSA> expected;
    BSA::MergeBSATypes mergeTypes;

    SUBCASE("MergeTypes: None")
    {
        mergeTypes = BSA::MergeBSATypes::None;
        expected   = std::vector{BSA{1'000'000, 2'000, StandardBsa},
                               BSA{1'000'000, 2'000, TexturesBsa},
                               BSA{1'000'000, 2'000, UncompressableBsa}};
    }
    SUBCASE("MergeTypes: Textures")
    {
        mergeTypes = BSA::MergeBSATypes::Textures;
        expected = std::vector{BSA{1'000'000, 4'000, StandardBsa}, BSA{1'000'000, 2'000, UncompressableBsa}};
    }
    SUBCASE("MergeTypes: Incompressible")
    {
        mergeTypes = BSA::MergeBSATypes::Incompressible;
        expected = std::vector{BSA{1'000'000, 4'000, UncompressableBsa}, BSA{1'000'000, 2'000, TexturesBsa}};
    }
    SUBCASE("MergeTypes: Both")
    {
        mergeTypes = BSA::MergeBSATypes::Both;
        expected   = std::vector<BSA>{BSA{1'000'000, 6'000, UncompressableBsa}};
    }

    BSA::mergeBSAs(input, mergeTypes);

    sort(input);
    sort(expected);
    CHECK_EQ(input, expected);
}

TEST_CASE("Merging lots of BSAs with mergeTypes: None")
{
    auto input = std::vector<BSA>{BSA{1'000, 20, TexturesBsa},
                                  BSA{1'000, 100, TexturesBsa},
                                  BSA{1'000, 200, TexturesBsa},
                                  BSA{1'000, 300, TexturesBsa},
                                  BSA{1'000, 25, StandardBsa},
                                  BSA{1'000, 50, StandardBsa},
                                  BSA{1'000, 400, StandardBsa},
                                  BSA{1'000, 500, StandardBsa},
                                  BSA{1'000, 10, UncompressableBsa},
                                  BSA{1'000, 50, UncompressableBsa},
                                  BSA{1'000, 100, UncompressableBsa},
                                  BSA{1'000, 1000, UncompressableBsa}};

    auto expected = std::vector<BSA>{BSA{1'000, 975, StandardBsa},
                                     BSA{1'000, 620, TexturesBsa},
                                     BSA{1'000, 1000, UncompressableBsa},
                                     BSA{1'000, 160, UncompressableBsa}};

    //Shuffling to prevent the function from getting an already ready vector
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(input.begin(), input.end(), g);

    BSA::mergeBSAs(input, BSA::MergeBSATypes::None);

    sort(input);
    sort(expected);
    CHECK_EQ(input, expected);
}
