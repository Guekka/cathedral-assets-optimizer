/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "bsa_saver.hpp"

namespace libbsarch {

bsa_saver_simple::bsa_saver_simple(bsa &bsa)
    : bsa_(bsa)
{}

void bsa_saver_simple::set_bsa(bsa &bsa)
{
    bsa_ = bsa;
}

void bsa_saver_simple::add_file(disk_blob &&blob)
{
    files_.emplace_back(std::move(blob));
}

const fs::path &bsa_saver_simple::get_save_path() const
{
    return save_path_;
}

void bsa_saver_simple::set_save_path(fs::path path)
{
    save_path_ = std::move(path);
}

bsa_archive_type_t bsa_saver_simple::get_save_type() const
{
    return save_type_;
}

void bsa_saver_simple::set_save_type(bsa_archive_type_t type)
{
    save_type_ = type;
}

void bsa_saver_simple::save(const fs::path &archive_path, bsa_archive_type_t type) const
{
    if (archive_path.empty())
        throw exception("Trying to save an archive with an empty path");

    if (type == baNone)
        throw exception("Trying to save an archive with a None type");

    bsa_entry_list entries;
    for (const auto &file : files_)
    {
        entries.add(file.path_in_archive);
    }

    auto create_result = bsa_create_archive(bsa_.get().get_unchecked().get(),
                                            archive_path.wstring().c_str(),
                                            type,
                                            entries.get_unchecked().get());
    checkResult(create_result);

    for (const auto &file : files_)
    {
        const auto &result = bsa_add_file_from_disk(bsa_.get().get_unchecked().get(),
                                                    file.path_in_archive.wstring().c_str(),
                                                    file.path_on_disk.wstring().c_str());

        checkResult(result);
    }

    const auto &result = bsa_save(bsa_.get().get_unchecked().get());
    checkResult(result);
}

void bsa_saver_simple::save() const
{
    save(save_path_, save_type_);
}

const std::vector<disk_blob> &bsa_saver_simple::get_file_list() const
{
    return files_;
}

void bsa_saver_complex::set_bsa(bsa &bsa)
{
    bsa_ = bsa;
}

void bsa_saver_complex::prepare(const fs::path &archive_path, bsa_entry_list entries, bsa_archive_type_t type)
{
    const auto &result = bsa_create_archive(bsa_.get().get_unchecked().get(),
                                            archive_path.wstring().c_str(),
                                            type,
                                            entries.get_unchecked().get());

    checkResult(result);
}

void bsa_saver_complex::add_file(const fs::path &path_in_archive, memory_blob &&memory_data)
{
    const auto &result = bsa_add_file_from_memory(bsa_.get().get_unchecked().get(),
                                                  path_in_archive.wstring().c_str(),
                                                  memory_data.buffer.size,
                                                  memory_data.buffer.data);
    checkResult(result);
}

void bsa_saver_complex::save() const
{
    const auto &result = bsa_save(bsa_.get().get_unchecked().get());
    checkResult(result);
}

} // namespace libbsarch
