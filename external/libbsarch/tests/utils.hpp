/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "../src/bsa.hpp"
#include <array>
#include <filesystem>

namespace libbsarch {

namespace fs = std::filesystem;

struct type_string
{
    bsa_archive_type_t type;
    std::string str;
};

inline const std::array standard_types{
    type_string{baFO3, "FO3"},
    type_string{baTES4, "TES4"},
    type_string{baSSE, "SSE"},
    type_string{baFO4, "FO4"},
};

inline const std::array texture_types{
    type_string{baFO4dds, "FO4dds"},
};

const fs::path &root_directory();

bool make_file(const fs::path &relative_path, const char *data);
bool make_file(const fs::path &relative_path, const std::string &data);

fs::path get_absolute_file_path(const fs::path &relative_path);

std::vector<fs::path> list_files(const fs::path &dir);

bool compare_dirs(const fs::path &p1, const fs::path &p2);
bool compare_files(const fs::path &p1, const fs::path &p2);

std::vector<fs::path> create_random_files(size_t count, const std::string &user_ext = "");

std::string random_string(size_t lenght);

} // namespace libbsarch
