/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "base_types.hpp"

namespace libbsarch {

exception::exception(const std::string &message)
    : runtime_error(message)
{}

exception::exception(const char *message)
    : runtime_error(message)
{}

extracted_data::extracted_data(bsa_result_buffer_t buffer_, bsa_archive_t parent)
    : buffer(buffer_)
    , parent_(parent)
{}

extracted_data::~extracted_data()
{
    if (parent_)
        bsa_file_data_free(parent_, buffer);
}

void *extracted_data::get_buffer()
{
    return buffer.data;
}

uint32_t extracted_data::get_size() const
{
    return buffer.size;
}

disk_blob::disk_blob(const fs::path &root_dir_, const fs::path &path_on_disk_)
    : path_in_archive(path_on_disk_.lexically_relative(root_dir_))
    , path_on_disk(path_on_disk_)
{}

disk_blob::disk_blob(const fs::path &path_in_archive_,
                     const fs::path &path_on_disk_,
                     [[maybe_unused]] bool decoy_parameter)
    : path_in_archive(path_in_archive_)
    , path_on_disk(path_on_disk_)
{}

namespace detail {

bsa_entry_list_wrapper::bsa_entry_list_wrapper()
{
    data_ = bsa_entry_list_create();
}

bsa_entry_list_wrapper::~bsa_entry_list_wrapper()
{
    bsa_entry_list_free(data_);
}

bsa_entry_list_t bsa_entry_list_wrapper::get() const
{
    return data_;
}

bsa_wrapper::bsa_wrapper()
{
    data_ = bsa_create();
}

bsa_wrapper::~bsa_wrapper()
{
    if (!data_)
        return;

    bsa_close(data_);
    bsa_free(data_);
}

bsa_wrapper::bsa_wrapper(detail::bsa_wrapper &&other)
{
    std::swap(data_, other.data_);
}

bsa_archive_t bsa_wrapper::get() const
{
    return data_;
}
} // namespace detail
} // namespace libbsarch
