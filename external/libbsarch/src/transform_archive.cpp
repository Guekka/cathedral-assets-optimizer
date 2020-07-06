/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "transform_archive.hpp"
#include "bsa_saver.hpp"

namespace libbsarch {

void transform_archive(
    const fs::path &source_path,
    const fs::path &target_path,
    std::function<libbsarch::memory_blob(const fs::path &, libbsarch::memory_blob &&)> callback,
    bsa_archive_type_t type)
{
    bsa source;
    source.load(source_path);

    transform_archive(source, target_path, callback, type);
}

void transform_archive(const bsa &source,
                       const fs::path target_path,
                       std::function<memory_blob(const fs::path &, memory_blob &&)> callback,
                       bsa_archive_type_t type)
{
    const auto &files = source.list_files();

    bsa_entry_list entries;
    std::for_each(files.cbegin(), files.cend(), [&entries](auto &&file) { entries.add(file); });

    if (type == baNone)
        type = source.get_type();

    bsa target;
    bsa_saver_complex target_saver(target);
    target_saver.prepare(target_path, std::move(entries), type);

    for (const auto &relative_path : files)
    {
        memory_blob input_blob = source.extract_to_memory(relative_path);
        auto output_blob = callback(relative_path, std::move(input_blob));
        target_saver.add_file(relative_path, std::move(output_blob));
    }
    target_saver.save();
}

} // namespace libbsarch
