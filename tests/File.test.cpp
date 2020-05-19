/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "utils.hpp"

using namespace CAO;

SCENARIO("Optimizing a file")
{
    GIVEN("A valid File")
    {
        TextureFile file;
        WHEN("The File is unchanged")
        {
            THEN("optimizedCurrentFile should be false") { CHECK(file.optimizedCurrentFile() == false); }
        }
        WHEN("The TextureFile is changed with 'optimizedFile' set to false")
        {
            auto file2 = getStandardTextureFile(false);
            THEN("optimizedCurrentFile should be false") { CHECK(file2->optimizedCurrentFile() == false); }
        }
        WHEN("The TextureFile is changed with default arguments")
        {
            auto file3 = getStandardTextureFile();
            THEN("optimizedCurrentFile should be true") { CHECK(file3->optimizedCurrentFile() == true); }
        }
    }
}
