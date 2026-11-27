#pragma once

#include <btu/bsa/archive.hpp>
#include <btu/bsa/pack.hpp>
#include <btu/bsa/settings.hpp>

#include <vector>

void write_single_archive(const btu::Path &directory_path,
                          btu::bsa::Archive &&archive,
                          const btu::bsa::Settings &bsa_sets,
                          const bool remove_files,
                          const bool make_override);

/// @brief Summary of what extracting a single archive would do, computed without reading its content to
/// disk or removing the archive.
struct BsaUnpackPreview
{
    btu::Path archive_path;
    /// Total number of files the archive contains.
    std::size_t total_files = 0;
    /// Absolute paths of loose files that already exist and would be skipped (not overwritten).
    std::vector<btu::Path> colliding_files;
};

/// @brief Logs files selected for packing using bethutil's metadata-only preview.
/// Does not read file contents, compress files, predict archive splitting, or modify files.
[[nodiscard]] auto preview_pack_directory(const btu::bsa::PackSettings &settings,
                                          bool remove_files) noexcept -> btu::bsa::PackPreview;

/// @brief Logs and returns what extracting the archive at @p archive_path into @p directory_path would
/// do, without extracting any file or removing the archive. Loose files that would collide (and thus be
/// skipped, matching the real extraction's overwrite_existing_files = false behavior) are reported.
[[nodiscard]] auto preview_unpack_archive(const btu::Path &archive_path,
                                          const btu::Path &directory_path) noexcept -> BsaUnpackPreview;
