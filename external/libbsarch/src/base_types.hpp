/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "libbsarch.h"
#include <cassert>
#include <filesystem>
#include <variant>

namespace libbsarch {
namespace fs = std::filesystem;

class exception : public std::runtime_error
{
public:
    explicit exception(const std::string &message);
    explicit exception(const char *message);
};

struct memory_blob
{
    explicit memory_blob(uint32_t buffer_size, bsa_buffer_t buffer_data, bsa_archive_t parent);
    explicit memory_blob(bsa_result_buffer_t buffer_, bsa_archive_t parent);

    memory_blob(const memory_blob &) = delete;
    memory_blob(memory_blob &&) = default;

    memory_blob &operator=(const memory_blob &) = delete;
    memory_blob &operator=(memory_blob &&) = default;

    ~memory_blob();

    bsa_result_buffer_t buffer;
    fs::path path_in_archive;

    void set_parent(bsa_archive_t parent);

private:
    bsa_archive_t parent_;
};

struct disk_blob
{
    explicit disk_blob(const fs::path &root_dir_, const fs::path &path_on_disk_);

    explicit disk_blob(const fs::path &path_in_archive_,
                       const fs::path &path_on_disk_,
                       [[maybe_unused]] bool decoy_parameter);

    disk_blob(const disk_blob &) = delete;
    disk_blob(disk_blob &&) = default;

    disk_blob &operator=(const disk_blob &) = delete;
    disk_blob &operator=(disk_blob &&) = default;

    fs::path path_in_archive;
    fs::path path_on_disk;
};

struct file_record
{
    bsa_file_record_t value;
};

namespace detail {

class bsa_entry_list_wrapper
{
public:
    bsa_entry_list_wrapper();
    ~bsa_entry_list_wrapper();

    bsa_entry_list_wrapper(const bsa_entry_list_wrapper &) = delete;
    bsa_entry_list_wrapper(bsa_entry_list_wrapper &&) = default;

    bsa_entry_list_wrapper &operator=(const bsa_entry_list_wrapper &) = delete;
    bsa_entry_list_wrapper &operator=(bsa_entry_list_wrapper &&) = default;

    bsa_entry_list_t get() const;

private:
    bsa_entry_list_t data_ = nullptr;
};

class bsa_wrapper
{
public:
    bsa_wrapper();
    ~bsa_wrapper();

    bsa_wrapper(const bsa_wrapper &) = delete;
    bsa_wrapper(bsa_wrapper &&);

    bsa_wrapper &operator=(const bsa_wrapper &) = delete;
    bsa_wrapper &operator=(bsa_wrapper &&);

    bsa_archive_t get() const;

private:
    bsa_archive_t data_ = nullptr;
};

} // namespace detail
} // namespace libbsarch
