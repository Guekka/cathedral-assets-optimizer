/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "utils.hpp"

#include "Commands/BSA/Utils/BSA.hpp"

using namespace CAO;

TEST_CASE("Merging BSAs of same type")
{
    ;

    auto input = std::vector<BSA>{BSA{1'000'000, 1'000, StandardBsa}, BSA{1'000'000, 998'000, StandardBsa}};
    auto const expected = std::vector<BSA>{BSA{1'000'000, 999'000, StandardBsa}};

    BSA::mergeBSAs(input, BSA::mergeDifferentTypes);

    CHECK_EQ(input, expected);
}

TEST_CASE("Merging BSAs of different types with mergeDifferentTypes enabled")
{
    auto input = std::vector<BSA>{BSA{1'000'000, 1'000, StandardBsa}, BSA{1'000'000, 998'000, TexturesBsa}};
    auto const expected = std::vector<BSA>{BSA{1'000'000, 999'000, StandardBsa}};

    BSA::mergeBSAs(input, BSA::mergeDifferentTypes);

    CHECK_EQ(input, expected);
}

TEST_CASE("Merging BSAs of different types with mergeDifferentTypes disabled")
{
    auto input    = std::vector<BSA>{BSA{1'000'000, 0, StandardBsa},
                                  BSA{1'000'000, 0, TexturesBsa},
                                  BSA{1'000'000, 0, UncompressableBsa}};
    auto expected = input;

    BSA::mergeBSAs(input, BSA::separateDifferentTypes);

    //Sorting to allow permutations
    auto sort = [](auto &&left, auto &&right) { return left.filesSize < right.filesSize; };
    input     = input | rx::sort(sort) | rx::to_vector();
    expected  = expected | rx::sort(sort) | rx::to_vector();

    CHECK_EQ(input, expected);
}

TEST_CASE("Merging lots of BSAs with mergeDifferentTypes disabled")
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

    BSA::mergeBSAs(input, BSA::separateDifferentTypes);

    //Sorting to allow permutations
    auto sort = [](auto &&left, auto &&right) { return left.filesSize < right.filesSize; };
    input     = input | rx::sort(sort) | rx::to_vector();
    expected  = expected | rx::sort(sort) | rx::to_vector();

    CHECK_EQ(input, expected);
}
