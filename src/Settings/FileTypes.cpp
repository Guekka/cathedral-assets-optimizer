/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "FileTypes.hpp"

namespace CAO {

std::optional<QString> FileTypes::isValid() const
{
    std::vector<std::string> allBSA = rx::chain(slBSAStandardFiles(),
                                                slBSATextureFiles(),
                                                slBSAUncompressibleFiles())
                                      | rx::to_vector();

    auto uniq = allBSA | rx::sort() | rx::uniq() | rx::to_vector();

    if (uniq.size() != allBSA.size()) //Duplicates
        return "slBSAStandardFiles, slBSATextureFiles and slBSATextureFiles have to contain different "
               "files.\n"
               "They cannot contain the same pattern";

    return std::nullopt;
}

} // namespace CAO
