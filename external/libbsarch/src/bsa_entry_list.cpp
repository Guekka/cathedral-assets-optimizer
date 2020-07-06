#include "bsa_entry_list.hpp"
#include "iostream"

namespace libbsarch {

void bsa_entry_list::add(const fs::path &file_path)
{
    const auto &result = bsa_entry_list_add(entries_.get(), file_path.wstring().c_str());
    checkResult(result);
}

uint32_t bsa_entry_list::count() const
{
    return bsa_entry_list_count(entries_.get());
}

std::vector<fs::path> bsa_entry_list::list() const
{
    std::vector<fs::path> list;
    uint32_t count = this->count();
    list.reserve(count);

    for (uint32_t i = 0; i < count; ++i)
    {
        wchar_t buffer[max_string_buffer_size];
        bsa_entry_list_get(entries_.get(), i, max_string_buffer_size, buffer);
        list.push_back(buffer);
    }
    return list;
}

const detail::bsa_entry_list_wrapper &bsa_entry_list::get_unchecked() const
{
    return entries_;
}

detail::bsa_entry_list_wrapper &bsa_entry_list::get_unchecked()
{
    return entries_;
}
} // namespace libbsarch
