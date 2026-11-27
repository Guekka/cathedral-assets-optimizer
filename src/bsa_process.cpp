#include "bsa_process.hpp"

#include <btu/bsa/pack.hpp>
#include <btu/bsa/plugin.hpp>
#include <btu/common/filesystem.hpp>
#include <plog/Log.h>

#include <format>
#include <string_view>
#include <vector>

/**
 * @brief Remove relative paths from a directory
 * Also removes directories made empty by the removal of the files
 * Remove the files case-insensitively (this is useful because the paths are lowercase)
 */
static auto remove_files_from_directory(const btu::Path &directory,
                                        std::span<const btu::Path> paths) noexcept -> size_t
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
        PLOGE << "Archive was written but cannot be opened, it is likely corrupted. Removing it.";
        btu::fs::remove(archive_path);

        // Verification failed: leave the loose source files untouched and report failure instead of
        // falling through to the removal logic below, which requires a verified archive.
        return;
    }

    if (remove_files)
    {
        PLOGI << "Removing files that were packed into the archive";
        auto relative_paths = flux::from_range(written_archive.value())
                                  .map([](const auto &file) { return file.first; })
                                  .to<std::vector<btu::Path>>();

        // close the archive before removing the files. The new value is unused
        written_archive.emplace(
            btu::bsa::Archive(btu::bsa::ArchiveVersion::fo3, btu::bsa::ArchiveType::Standard));
        const auto count = remove_files_from_directory(directory_path, relative_paths);
        PLOGI << std::format("Attempted to remove {} files, {} were removed", relative_paths.size(), count);
    }

    if (make_override)
    {
        const auto override_path = std::move(archive_path).replace_extension(".override");

        auto res = btu::common::write_file_new(override_path, {});
        if (!res.has_value() && res.error() != std::errc::file_exists) // we're good if file already exists
        {
            PLOGE << "Failed to create override file: " << res.error();
        }
    }
}

auto preview_pack_directory(const btu::bsa::PackSettings &settings,
                            const bool remove_files) noexcept -> btu::bsa::PackPreview
{
    auto preview = btu::bsa::list_pack_preview(settings);

    const auto log_files = [](const std::string_view archive_group, const std::vector<btu::Path> &files) {
        for (const auto &relative_path : files)
            PLOGI << std::format("Dry run: would include in {} archive(s): {}",
                                 archive_group,
                                 relative_path.string());
    };
    log_files("standard", preview.standard_files);
    log_files("texture", preview.texture_files);

    PLOGI << std::format(
        "Dry run: selected {} standard files and {} texture files; archive count and splitting "
        "are not predicted because they depend on resulting file sizes",
        preview.standard_files.size(),
        preview.texture_files.size());

    if (remove_files)
    {
        PLOGI << "Dry run: would remove packed loose files only after successfully writing and verifying "
                 "their archives";
    }

    return preview;
}

auto preview_unpack_archive(const btu::Path &archive_path, const btu::Path &directory_path) noexcept
    -> tl::expected<btu::bsa::UnpackPreview, btu::common::Error>
{
    auto preview = btu::bsa::list_unpack_preview(
        {.file_path = archive_path, .overwrite_existing_files = false, .extract_to_dir = directory_path});
    if (!preview)
    {
        PLOGE << "Dry run: failed to open archive " << archive_path.string()
              << " for preview. Reason: " << preview.error().message();
        return tl::make_unexpected(preview.error());
    }

    for (const auto &relative_path : preview->files_to_extract)
        PLOGI << std::format("Dry run: would extract {}", relative_path.string());
    for (const auto &relative_path : preview->skipped_files)
        PLOGI << std::format("Dry run: would skip extracting {} (a file already exists at this location)",
                             relative_path.string());

    PLOGI << std::format(
        "Dry run: archive {} contains {} files, {} would be skipped because they already exist",
        archive_path.filename().string(),
        preview->files_to_extract.size() + preview->skipped_files.size(),
        preview->skipped_files.size());

    return preview;
}
