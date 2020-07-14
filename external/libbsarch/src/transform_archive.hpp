/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "bsa.hpp"
#include <cstddef>
#include <functional>

namespace libbsarch {

using transform_callback = std::function<std::vector<std::byte>(const fs::path &relative_path, extracted_data &&blob)>;

void transform_archive(const fs::path &source_path,
                       const fs::path &target_path,
                       transform_callback callback,
                       bsa_archive_type_t type = bsa_archive_type_t::baNone);

void transform_archive(const bsa &source,
                       const fs::path &target_path,
                       transform_callback callback,
                       bsa_archive_type_t type = bsa_archive_type_t::baNone);

} // namespace libbsarch
