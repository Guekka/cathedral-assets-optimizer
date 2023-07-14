/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "FileTypes.hpp"

namespace cao {

std::optional<QString> FileTypes::isValid() const
{
    /*
    std::vector<std::string> allBSA = rx::chain(slBSAStandardFiles(),
                                                slBSATextureFiles(),
                                                slBSAUncompressibleFiles())
                                      | rx::to_vector();

    auto uniq = allBSA | rx::sort() | rx::uniq() | rx::to_vector();

    if (uniq.size() != allBSA.size()) //Duplicates
        return tr("slBSAStandardFiles, slBSATextureFiles and slBSATextureFiles have to contain different "
                  "files.\n"
                  "They cannot contain the same pattern");

     */

    // FIXME
    return std::nullopt;
}

FileTypes::FileTypes(const FileTypes &other)
{
    json_ = other.json_;
}

FileTypes::FileTypes(FileTypes &&other) noexcept
{
    json_ = std::move(other.json_);
}

FileTypes &FileTypes::operator=(const FileTypes &other)
{
    if (this != &other)
        json_ = other.json_;

    return *this;
}

FileTypes &FileTypes::operator=(FileTypes &&other) noexcept
{
    if (this != &other)
        json_ = std::move(other.json_);

    return *this;
}

bool FileTypes::match(const std::vector<std::u8string> &patterns, const std::u8string &str) const
{
    /*
    auto name = str.toStdString();

    auto match = [&name](const std::string &str) {
        return isMatch(name, pattern{str}, case_insensitive);
    };

    return any_of(patterns, match);
*/
    return false; // FIXME
}

} // namespace cao
