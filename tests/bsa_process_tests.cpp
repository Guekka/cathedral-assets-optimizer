/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "bsa_process.hpp"

#include <btu/bsa/pack.hpp>
#include <btu/bsa/plugin.hpp>
#include <btu/common/games.hpp>
#include <doctest/doctest.h>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <numeric>
#include <string>
#include <system_error>
#include <vector>

namespace cao {
namespace {
class TemporaryDirectory
{
public:
    explicit TemporaryDirectory(std::string_view tag)
    {
        const auto suffix = std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        path_             = std::filesystem::temp_directory_path()
                / ("cao-bsa-process-" + std::string(tag) + "-" + suffix);
        std::filesystem::create_directories(path_);
    }

    TemporaryDirectory(const TemporaryDirectory &)                     = delete;
    auto operator=(const TemporaryDirectory &) -> TemporaryDirectory & = delete;

    ~TemporaryDirectory()
    {
        auto error = std::error_code{};
        std::filesystem::remove_all(path_, error);
    }

    [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & { return path_; }

private:
    std::filesystem::path path_;
};

void write_file(const std::filesystem::path &path, std::string_view content)
{
    std::filesystem::create_directories(path.parent_path());
    std::ofstream file(path, std::ios::binary);
    file.write(content.data(), static_cast<std::streamsize>(content.size()));
}

[[nodiscard]] auto read_file(const std::filesystem::path &path) -> std::string
{
    std::ifstream file(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}

[[nodiscard]] auto count_entries(const std::filesystem::path &dir) -> std::size_t
{
    if (!std::filesystem::exists(dir))
        return 0;

    return static_cast<std::size_t>(std::distance(std::filesystem::recursive_directory_iterator(dir),
                                                  std::filesystem::recursive_directory_iterator{}));
}
} // namespace

TEST_CASE("preview_pack_directory lists selected files without writing or deleting anything")
{
    const auto source      = TemporaryDirectory("pack-src");
    const auto &source_dir = source.path();

    write_file(source_dir / "meshes" / "a.nif", "mesh-a-content");
    write_file(source_dir / "meshes" / "sub" / "b.nif", "mesh-b-content");
    write_file(source_dir / "meshes" / "filtered.nif", "filtered-content");
    write_file(source_dir / "textures" / "c.dds", "texture-c-content");

    const auto &bsa_sets = btu::bsa::Settings::get(btu::Game::SSE);

    const auto entries_before = count_entries(source_dir);

    const auto settings = btu::bsa::PackSettings{.input_dir     = source_dir,
                                                 .game_settings = bsa_sets,
                                                 .compress      = btu::bsa::Compression::Yes,
                                                 .allow_file_pred =
                                                     [](const btu::Path &,
                                                        const std::filesystem::directory_entry &entry) {
                                                         return entry.path().filename() != "filtered.nif";
                                                     }};
    const auto preview  = preview_pack_directory(settings, /*remove_files=*/true);

    auto standard_files = preview.standard_files;
    auto texture_files  = preview.texture_files;
    std::ranges::sort(standard_files);
    std::ranges::sort(texture_files);
    const auto expected_standard_files = std::vector<btu::Path>{"meshes/a.nif", "meshes/sub/b.nif"};
    const auto expected_texture_files  = std::vector<btu::Path>{"textures/c.dds"};
    CHECK(standard_files == expected_standard_files);
    CHECK(texture_files == expected_texture_files);

    // Nothing should have been written to, or removed from, the source directory: no archive appeared,
    // and the original loose files (and their content) are untouched.
    CHECK(count_entries(source_dir) == entries_before);
    CHECK(std::filesystem::exists(source_dir / "meshes" / "a.nif"));
    CHECK(std::filesystem::exists(source_dir / "meshes" / "sub" / "b.nif"));
    CHECK(std::filesystem::exists(source_dir / "meshes" / "filtered.nif"));
    CHECK(std::filesystem::exists(source_dir / "textures" / "c.dds"));
    CHECK(read_file(source_dir / "meshes" / "a.nif") == "mesh-a-content");
    CHECK(read_file(source_dir / "meshes" / "sub" / "b.nif") == "mesh-b-content");
    CHECK(read_file(source_dir / "meshes" / "filtered.nif") == "filtered-content");
    CHECK(read_file(source_dir / "textures" / "c.dds") == "texture-c-content");

    for (const auto &entry : std::filesystem::recursive_directory_iterator(source_dir))
        CHECK_FALSE(entry.path().extension() == bsa_sets.extension);
}

TEST_CASE("preview_unpack_archive reports planned extraction collisions without extracting or removing "
          "the archive")
{
    const auto pack_source      = TemporaryDirectory("unpack-pack-src");
    const auto &pack_source_dir = pack_source.path();

    write_file(pack_source_dir / "meshes" / "a.nif", "original-a-content");
    write_file(pack_source_dir / "meshes" / "b.nif", "original-b-content");

    const auto &bsa_sets = btu::bsa::Settings::get(btu::Game::SSE);

    const auto archive_dir = TemporaryDirectory("unpack-archive-dir");

    std::filesystem::path archive_path;
    btu::bsa::pack(btu::bsa::PackSettings{.input_dir     = pack_source_dir,
                                          .game_settings = bsa_sets,
                                          .compress      = btu::bsa::Compression::No})
        .for_each([&](auto archive) {
            REQUIRE(archive_path.empty()); // the two files above should fit in a single archive
            const auto name_opt = btu::bsa::find_archive_name(archive_dir.path(), bsa_sets, archive.type());
            REQUIRE(name_opt.has_value());
            archive_path = std::move(name_opt).value();

            REQUIRE(std::move(archive).write(archive_path));
        });

    REQUIRE(std::filesystem::exists(archive_path));
    const auto archive_size_before = std::filesystem::file_size(archive_path);

    const auto destination      = TemporaryDirectory("unpack-dst");
    const auto &destination_dir = destination.path();

    // Simulate a loose file that already exists where "meshes/a.nif" would be extracted.
    write_file(destination_dir / "meshes" / "a.nif", "pre-existing-loose-content");

    const auto preview = preview_unpack_archive(archive_path, destination_dir);

    REQUIRE(preview.has_value());
    CHECK(preview->files_to_extract == std::vector<btu::Path>{"meshes/b.nif"});
    CHECK(preview->skipped_files == std::vector<btu::Path>{"meshes/a.nif"});

    // The archive must still exist, byte-for-byte, and nothing should have been extracted: the
    // colliding file keeps its original content, and the non-colliding file was never created.
    REQUIRE(std::filesystem::exists(archive_path));
    CHECK(std::filesystem::file_size(archive_path) == archive_size_before);
    CHECK(read_file(destination_dir / "meshes" / "a.nif") == "pre-existing-loose-content");
    CHECK_FALSE(std::filesystem::exists(destination_dir / "meshes" / "b.nif"));
}

TEST_CASE("preview_unpack_archive reports a failure without crashing when the archive cannot be opened")
{
    const auto directory = TemporaryDirectory("unpack-bad-archive");

    const auto bad_archive_path = directory.path() / "not_a_real_archive.bsa";
    write_file(bad_archive_path, "this is not a valid bsa file");

    const auto preview = preview_unpack_archive(bad_archive_path, directory.path());

    CHECK_FALSE(preview.has_value());

    // Nothing should have been touched: the bogus archive is still there, untouched.
    CHECK(std::filesystem::exists(bad_archive_path));
}

} // namespace cao
