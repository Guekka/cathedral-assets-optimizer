#pragma once

#include "libbsarch.hpp"
#include <vector>

namespace libbsarch {
class bsa_entry_list
{
public:
    void add(const fs::path &file_path);
    uint32_t count() const;
    std::vector<fs::path> list() const;

    const detail::bsa_entry_list_wrapper &get_unchecked() const;
    detail::bsa_entry_list_wrapper &get_unchecked();

private:
    detail::bsa_entry_list_wrapper entries_;
};
} // namespace libbsarch
