#include "bsa_process.hpp"

#include <btu/bsa/pack.hpp>
#include <btu/bsa/plugin.hpp>
#include <btu/common/filesystem.hpp>
#include <plog/Log.h>

/**
 * @brief Remove relative paths from a directory
 * Also removes directories made empty by the removal of the files
 * Remove the files case-insensitively (this is useful because the paths are lowercase)
 */
auto remove_files_from_directory(const btu::Path &directory, std::span<const btu::Path> paths) noexcept
    -> size_t
{
    const auto files_to_remove = btu::common::find_matching_paths_icase(directory, paths);

    size_t count{};
    std::error_code ec; // just ignore errors

    for (const auto &real_path : files_to_remove)
    {
        count += btu::fs::remove(real_path, ec) ? 1 : 0;

        // Remove the parent directory if it is empty
        // TODO: improve this: sometimes empty dirs are left
        if (auto parent_dir = real_path.parent_path(); btu::fs::is_empty(parent_dir))
            btu::fs::remove(parent_dir, ec);
    }

    return count;
}

void write_single_archive(const btu::Path &directory_path,
                          btu::bsa::Archive &&archive,
                          const btu::bsa::Settings &bsa_sets,
                          const bool remove_files)
{
    const auto archive_path = find_archive_name(directory_path, bsa_sets, archive.type());
    if (!archive_path)
    {
        PLOGE << "Failed to find a name for the archive, skipping";
        return;
    }

    if (const bool success = std::move(archive).write(archive_path.value()); !success)
    {
        PLOGE << "Failed to write archive " << archive_path.value().string();
        return;
    }

    // we open the archive again to make sure the files are written
    auto written_archive = btu::bsa::Archive::read(archive_path.value());
    if (!written_archive)
    {
        PLOGE << "Archive was written but cannot be opened, it is likely corrupted. Removing it.";
        btu::fs::remove(archive_path.value());
    }

    if (remove_files)
    {
        PLOGI << "Removing files that were packed into the archive";
        auto relative_paths = flux::from_range(written_archive.value())
                                  .map([](const auto &file) { return file.first; })
                                  .to<std::vector<btu::Path>>();

        written_archive.reset(); // close the archive before removing the files
        const auto count = remove_files_from_directory(directory_path, relative_paths);
        PLOGI << std::format("Attempted to remove {} files, {} were removed", relative_paths.size(), count);
    }
}
