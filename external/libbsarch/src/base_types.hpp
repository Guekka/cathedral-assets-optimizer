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

class extracted_data
{
public:
    explicit extracted_data(bsa_result_buffer_t buffer_, bsa_archive_t parent);

    extracted_data(const extracted_data &) = delete;
    extracted_data(extracted_data &&) = default;

    extracted_data &operator=(const extracted_data &) = delete;
    extracted_data &operator=(extracted_data &&) = default;

    ~extracted_data();

    void *get_buffer();
    uint32_t get_size() const;

private:
    bsa_result_buffer_t buffer;
    bsa_archive_t parent_;
};

inline std::vector<std::byte> to_vector(extracted_data &&data)
{
    size_t size = data.get_size();
    auto buffer = static_cast<std::byte *>(data.get_buffer());
    return std::vector(buffer, buffer + size);
}

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
    bsa_entry_list_wrapper(bsa_entry_list_wrapper &&);

    bsa_entry_list_wrapper &operator=(const bsa_entry_list_wrapper &) = delete;
    bsa_entry_list_wrapper &operator=(bsa_entry_list_wrapper &&);

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
