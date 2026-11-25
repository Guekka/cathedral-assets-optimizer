#include "bsa_process.hpp"

#include <btu/bsa/pack.hpp>
#include <btu/bsa/plugin.hpp>
#include <btu/common/filesystem.hpp>
#include <plog/Log.h>

#include <fstream>
#include <format>
#include <string>

/**
 * @brief Remove relative paths from a directory
 * Also removes directories made empty by the removal of the files
 * Remove the files case-insensitively (this is useful because the paths are lowercase)
 */
static auto remove_files_from_directory(const btu::Path &directory, std::span<const btu::Path> paths) noexcept
    -> size_t
{
    const auto files_to_remove = btu::common::find_matching_paths_icase(directory, paths);

    size_t count{};
    std::error_code ec; // just ignore errors

    for (const auto &real_path : files_to_remove)
    {
        count += btu::fs::remove(real_path, ec) ? 1 : 0;

        // Remove empty parent directories recursively
        auto parent_dir = real_path.parent_path();
        while (parent_dir != directory && btu::fs::is_empty(parent_dir))
        {
            btu::fs::remove(parent_dir, ec);
            parent_dir = parent_dir.parent_path();
        }
    }

    return count;
}

void write_single_archive(const btu::Path &directory_path,
                          btu::bsa::Archive &&archive,
                          const btu::bsa::Settings &bsa_sets,
                          const bool remove_files,
                          const bool make_override)
{
    const auto archive_path_opt = find_archive_name(directory_path, bsa_sets, archive.type());
    if (!archive_path_opt)
    {
        PLOGE << "Failed to find a name for the archive, skipping";
        return;
    }
    auto archive_path = std::move(archive_path_opt).value();

    if (const bool success = std::move(archive).write(archive_path); !success)
    {
        PLOGE << "Failed to write archive " << archive_path.string();
        return;
    }

    // we open the archive again to make sure the files are written
    auto written_archive = btu::bsa::Archive::read(archive_path);
    if (!written_archive)
    {
        std::error_code removal_error;
        const bool removed = btu::fs::remove(archive_path, removal_error);
        auto removal_status = std::string{"The rejected archive was removed."};
        if (removal_error)
            removal_status = std::format("The rejected archive could not be removed: {}",
                                         removal_error.message());
        else if (!removed)
            removal_status = "The rejected archive was already absent.";

        PLOGE << std::format("Could not verify generated archive {}: {}. {} Original files were kept.",
                             archive_path.string(),
                             written_archive.error().message(),
                             removal_status);
        return;
    }

    if (remove_files)
    {
        PLOGI << "Removing files that were packed into the archive";
        auto relative_paths = flux::from_range(written_archive.value())
                                  .map([](const auto &file) { return file.first; })
                                  .to<std::vector<btu::Path>>();

        // close the archive before removing the files. The new value is unused
        written_archive.emplace(btu::bsa::Archive(btu::bsa::ArchiveVersion::fo3, btu::bsa::ArchiveType::Standard));
        const auto count = remove_files_from_directory(directory_path, relative_paths);
        PLOGI << std::format("Attempted to remove {} files, {} were removed", relative_paths.size(), count);
    }

    if (make_override)
    {
        const auto override_path = std::move(archive_path).replace_extension(".override");

        btu::common::write_file_new(override_path, {}).map_error([](const btu::common::Error &e) {
            if (e == std::errc::file_exists)
                return; // this is fine

            PLOGE << "Failed to create override file: " << e;
        });
    }
}
