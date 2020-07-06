/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "utils.hpp"
#include <filesystem>

int main()
{
    doctest::Context context;

    int res = context.run();

    if (context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
        return res;           // propagate the result of the tests

    //Cleanup test dir
    //std::filesystem::remove_all(libbsarch::root_directory());
}
