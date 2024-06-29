#include "bsa_process.hpp"

#include <btu/bsa/pack.hpp>
#include <btu/bsa/plugin.hpp>
#include <btu/bsa/unpack.hpp>
#include <plog/Log.h>

// TODO: upstream
void make_dummy_plugins(const btu::Path &directory, const btu::bsa::Settings &sets)
{
    auto plugins = list_plugins(btu::fs::directory_iterator(directory), btu::fs::directory_iterator{}, sets);
    clean_dummy_plugins(plugins, sets);
    auto archives = list_archive(btu::fs::directory_iterator(directory), btu::fs::directory_iterator{}, sets);
    btu::bsa::make_dummy_plugins(archives, sets);
}

// TODO: upstream
[[nodiscard]] auto find_matching_paths_icase(const btu::Path &directory,
                                             std::span<const btu::Path> paths) noexcept
    -> std::vector<btu::Path>
{
#ifdef _WIN32
    // Windows is case-insensitive by default. Only check if the file exists
    return flux::from(paths)
        .map([&directory](const btu::Path &path) { return directory / path; })
        .filter([](const auto &path) { return btu::fs::exists(path); })
        .to<std::vector<btu::Path>>();
#else
    // On Linux, we need to make a case-insensitive map of the files in the directory. This is slow.
    // We also assume there is only one file with the same lowercase path. This is probably fine since
    // mods were made for Windows and Windows is case-insensitive.
    const auto files_in_directory
        = flux::from_range(btu::fs::recursive_directory_iterator(directory))
              .map([&directory](const btu::fs::directory_entry &entry) {
                  const auto &path         = entry.path();
                  const auto relative_path = btu::fs::relative(path, directory);
                  return std::pair{btu::common::to_lower(relative_path.u8string()), path};
              })
              .to<std::unordered_map<std::u8string, btu::Path>>();

    return flux::from(paths)
        .map([&files_in_directory](const btu::Path &path) -> std::optional<btu::Path> {
            const auto lower_path = btu::common::to_lower(path.u8string());
            if (const auto it = files_in_directory.find(lower_path); it != files_in_directory.end())
                return it->second;
            return {};
        })
        .filter([](const auto &path) { return path.has_value(); })
        .map([](const auto &path) { return path.value(); })
        .to<std::vector<btu::Path>>();
#endif
}

/**
 * @brief Remove relative paths from a directory
 * Also removes directories made empty by the removal of the files
 * Remove the files case-insensitively (this is useful because the paths are lowercase)
 */
auto remove_files_from_directory(const btu::Path &directory, std::span<const btu::Path> paths) noexcept
    -> size_t
{
    const auto files_to_remove = find_matching_paths_icase(directory, paths);

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

    if (const bool success = std::move(archive).write(archive_path->full_path()); !success)
    {
        PLOGE << "Failed to write archive " << archive_path->full_path().string();
        return;
    }

    // we open the archive again to make sure the files are written
    auto written_archive = btu::bsa::Archive::read(archive_path->full_path());
    if (!written_archive)
    {
        PLOGE << "Archive was written but cannot be opened, it is likely corrupted. Removing it.";
        btu::fs::remove(archive_path->full_path());
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
