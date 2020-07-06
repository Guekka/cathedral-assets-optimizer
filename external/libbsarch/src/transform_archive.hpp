/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "bsa.hpp"
#include <functional>

namespace libbsarch {

void transform_archive(const fs::path &source_path,
                       const fs::path &target_path,
                       std::function<memory_blob(const fs::path &relative_path, memory_blob &&blob)> callback,
                       bsa_archive_type_t type = bsa_archive_type_t::baNone);

void transform_archive(const bsa &source,
                       const fs::path target_path,
                       std::function<memory_blob(const fs::path &relative_path, memory_blob &&blob)> callback,
                       bsa_archive_type_t type = bsa_archive_type_t::baNone);

} // namespace libbsarch
