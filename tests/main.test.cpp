/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"
#include "utils.hpp"

int main(int argc, char **argv)
{
    initSettings();

    doctest::Context context;
    int res = context.run();  // run

    if (context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
        return res;           // propagate the result of the tests

    int client_stuff_return_code = 0;
    // your program - if the testing framework is integrated in your production code

    return res + client_stuff_return_code; // the result from doctest is propagated here as well
}
