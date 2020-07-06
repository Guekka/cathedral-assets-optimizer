/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "../src/bsa_saver.hpp"
#include "doctest.h"
#include "utils.hpp"

using namespace libbsarch;

TEST_CASE("Test read")
{
    //"test_read" dir have to be copied alongside the test exe
    const fs::path root = "test_read";
    const fs::path generated = root / "generated_temp_files";

    auto files = list_files(generated);

    std::transform(files.cbegin(), files.cend(), files.begin(), [&root](auto &&path) {
        return path.lexically_relative(root);
    });

    for (auto type : standard_types)
    {
        bsa source;
        source.load(root / (type.str + ".bsa"));
        const auto listed_files = source.list_files();

        CHECK(std::is_permutation(listed_files.cbegin(), listed_files.cend(), files.cbegin(), files.cend()));

        const fs::path &dir = root_directory() / ("extraction - " + type.str);
        source.extract_all_to_disk(dir, bsa::overwrite_current_files);
        CHECK(compare_dirs(generated, dir));
    }
}
