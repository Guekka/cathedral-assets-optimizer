/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "utils.hpp"
#include <fstream>
#include <random>

namespace libbsarch {

const fs::path &root_directory()
{
    static const fs::path root_dir = fs::temp_directory_path() / "libbsarch_tests";
    return root_dir;
}

bool make_file(const fs::path &relative_path, const char *data = "")
{
    auto absolute_dir = root_directory() / relative_path.parent_path();
    fs::create_directories(absolute_dir);

    std::fstream file(root_directory() / relative_path, std::ios::out);
    if (!file.is_open())
        return false;

    file << data;

    file.close();
    return true;
}

bool make_file(const fs::path &relative_path, const std::string &data)
{
    return make_file(relative_path, data.c_str());
}

fs::path get_absolute_file_path(const fs::path &relative_path)
{
    return root_directory() / relative_path;
}

bool compare_dirs(const fs::path &p1, const fs::path &p2)
{
    auto files_one = list_files(p1);
    auto files_two = list_files(p2);

    std::sort(files_one.begin(), files_one.end());
    std::sort(files_two.begin(), files_two.end());

    if (files_one.size() != files_two.size())
        return false;

    for (size_t i = 0; i < files_one.size(); i++)
    {
        auto absolute_one = fs::absolute(files_one[i]);
        auto absolute_two = fs::absolute(files_two[i]);

        if (!compare_files(absolute_one, absolute_two))
            return false;
    }
    return true;
}

bool compare_files(const fs::path &p1, const fs::path &p2)
{
    std::ifstream f1(p1, std::ifstream::binary | std::ifstream::ate);
    std::ifstream f2(p2, std::ifstream::binary | std::ifstream::ate);

    if (f1.fail() || f2.fail())
    {
        return false; //file problem
    }

    if (f1.tellg() != f2.tellg())
    {
        return false; //size mismatch
    }

    //seek back to beginning and use std::equal to compare contents
    f1.seekg(0, std::ifstream::beg);
    f2.seekg(0, std::ifstream::beg);

    return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                      std::istreambuf_iterator<char>(),
                      std::istreambuf_iterator<char>(f2.rdbuf()),
                      std::istreambuf_iterator<char>());
}

std::vector<fs::path> create_random_files(size_t count, const std::string &user_ext)
{
    std::vector<fs::path> paths;
    static const fs::path dir = "generated_temp_files";
    fs::create_directories(dir);

    for (size_t i = 0; i < count; ++i)
    {
        const fs::path folder = dir / random_string(10);
        const fs::path filename = random_string(10);
        const std::string ext = !user_ext.empty() ? user_ext : random_string(4);

        const fs::path path = folder / (filename.string() + '.' + ext);
        make_file(path, random_string(1000));
        paths.emplace_back(path);
    }
    return paths;
}

std::string random_string(size_t lenght)
{
    static std::random_device rd;
    static std::default_random_engine mt(rd());
    static std::uniform_int_distribution<int> dist(0, 25);

    std::string result;
    result.reserve(lenght);

    for (size_t i = 0; i < lenght; ++i)
        result.push_back('a' + dist(mt));

    return result;
}

std::vector<fs::path> list_files(const fs::path &dir)
{
    std::vector<fs::path> files;
    files.reserve(100);

    for (const auto &p : fs::recursive_directory_iterator(dir))
        if (p.is_regular_file())
            files.emplace_back(p);

    return files;
}

} // namespace libbsarch
