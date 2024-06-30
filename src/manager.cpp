/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "manager.hpp"

#include "bsa_process.hpp"
#include "main_process.hpp"
#include "settings/settings.hpp"

#include <btu/bsa/pack.hpp>
#include <btu/bsa/plugin.hpp>
#include <btu/bsa/settings.hpp>
#include <btu/bsa/unpack.hpp>
#include <btu/common/string.hpp>
#include <btu/esp/functions.hpp>
#include <btu/modmanager/mod_manager.hpp>
#include <fmt/chrono.h>
#include <plog/Log.h>

#include <filesystem>
#include <mutex>
#include <utility>

namespace cao {

Manager::Manager(Settings settings)
    : settings_(std::move(settings))
{
}

constexpr auto k_bad_file_ext = ".caobad";

void rename_bad_file(const std::filesystem::path &file_path)
{
    // add k_bad_file_ext to the file name
    auto new_file_path = file_path;
    new_file_path += k_bad_file_ext;

    try
    {
        PLOG_ERROR << fmt::format("File {} is bad. Renaming to {}",
                                  file_path.string(),
                                  new_file_path.string());
        btu::fs::rename(file_path, new_file_path);
    }
    catch (const std::exception &e)
    {
        PLOG_ERROR << fmt::format("Failed to rename file {} to {}: {}",
                                  file_path.string(),
                                  new_file_path.string(),
                                  e.what());
    }
}

struct PluginInfo
{
    std::vector<std::u8string> headparts;
    std::vector<std::u8string> landscape_textures;
};

[[nodiscard]] auto get_plugin_info(const btu::modmanager::ModFolder &mod) -> PluginInfo
{
    PluginInfo result;
    std::mutex mutex;

    mod.iterate(
        [&result, &mutex, &mod](const btu::Path &relative_path) {
            static constexpr auto k_plugin_exts = std::to_array({".esp", ".esm", ".esl"});
            if (!btu::common::contains(k_plugin_exts, relative_path.extension()))
                return;

            PLOG_INFO << fmt::format("Parsing plugin {}", relative_path.string());

            const auto absolute_path = mod.path() / relative_path;
            auto headparts           = btu::esp::list_headparts(absolute_path);
            auto landscape_textures  = btu::esp::list_landscape_textures(absolute_path);

            if (!headparts && !landscape_textures)
            {
                PLOGV << fmt::format("Plugin {} has no headparts or landscape textures",
                                     relative_path.string());
            }

            const auto lock = std::scoped_lock(mutex);

            if (headparts)
                result.headparts.insert(result.headparts.end(), headparts->begin(), headparts->end());

            if (landscape_textures)
                result.landscape_textures.insert(result.landscape_textures.end(),
                                                 landscape_textures->begin(),
                                                 landscape_textures->end());
        },
        // plugins cannot be in archives
        [](const btu::Path &) {});

    return result;
}

void Manager::unpack_directory(const std::filesystem::path &directory_path) const
{
    PLOG_INFO << fmt::format("Extracting archives in {}", directory_path.string());

    std::vector archives(btu::fs::directory_iterator(directory_path), btu::fs::directory_iterator{});
    erase_if(archives, [](const auto &file) {
        return !btu::common::contains(btu::bsa::k_archive_extensions, file.path().extension());
    });

    emit files_counted(archives.size());

    std::ranges::for_each(archives, [this](const auto &entry) {
        const auto res = unpack(btu::bsa::UnpackSettings{
            .file_path                = entry.path(),
            .remove_arch              = true,
            .overwrite_existing_files = false,
            .root_opt                 = nullptr,
        });

        emit files_processed(entry.path(), 1);

        switch (res)
        {
            case btu::bsa::UnpackResult::Success: break;
            case btu::bsa::UnpackResult::UnreadableArchive:
                PLOGE << "Unreadable archive: " << entry.path().string();
                break;
            case btu::bsa::UnpackResult::FailedToDeleteArchive:
                PLOGE << "Failed to delete archive: " << entry.path().string();
                break;
        }
    });
}

void apply_plugin_info(PerFileSettings &sets, const PluginInfo &info)
{
    sets.nif.headpart_meshes.insert(sets.nif.headpart_meshes.end(),
                                    info.headparts.begin(),
                                    info.headparts.end());

    btu::common::remove_duplicates(sets.nif.headpart_meshes);

    sets.tex.landscape_textures.insert(sets.tex.landscape_textures.end(),
                                       info.landscape_textures.begin(),
                                       info.landscape_textures.end());

    btu::common::remove_duplicates(sets.tex.landscape_textures);
}

void apply_plugin_info(Settings &sets, const PluginInfo &info)
{
    auto &profile = sets.current_profile();

    std::ranges::for_each(profile.per_file_settings(),
                          [&info](auto &per_file_sets) { apply_plugin_info(*per_file_sets, info); });
}

// TODO: think about adding files to existing BSAs
void Manager::pack_directory(const std::filesystem::path &directory_path) const
{
    PLOG_INFO << fmt::format("Packing directory {}", directory_path.string());
    emit files_counted(0);
    emit files_processed("archive packing", 0);

    auto bsa_sets = btu::bsa::Settings::get(settings_.current_profile().target_game);

    pack(btu::bsa::PackSettings{
             .input_dir     = directory_path,
             .game_settings = bsa_sets,
             .compress      = btu::bsa::Compression::Yes,
         })
        .for_each([&](auto archive) {
            try
            {
                write_single_archive(directory_path,
                                     std::move(archive),
                                     bsa_sets,
                                     settings_.current_profile().bsa_remove_files);
            }
            catch (const std::exception &e)
            {
                PLOGE << "Failed to pack archive: " << e.what();
            }
        });

    if (settings_.current_profile().bsa_make_dummy_plugins)
        remake_dummy_plugins(directory_path, bsa_sets);
}

void Manager::emit_progress_rate_limited(const btu::Path &path)
{
    constexpr auto emit_every = std::chrono::milliseconds(500);

    const auto now = std::chrono::steady_clock::now();
    if ((now - last_emission_.load()) > emit_every)
    {
        last_emission_.store(now);
        emit files_processed(path, ++files_processed_since_last_emissions_);
    }
}

class ModTransformer final : public btu::modmanager::ModFolderTransformer
{
public:
    using ProgressCallback = std::function<void(const btu::Path &)>;

private:
    Settings settings_;
    ProgressCallback progress_callback_;

public:
    ModTransformer(Settings settings, ProgressCallback progress_callback)
        : settings_(std::move(settings))
        , progress_callback_(std::move(progress_callback))
    {
    }

    [[nodiscard]] auto archive_too_large(const btu::Path &archive_path, ArchiveTooLargeState state) noexcept
        -> ArchiveTooLargeAction override
    {
        switch (state)
        {
            case ArchiveTooLargeState::BeforeProcessing:
            {
                PLOG_ERROR << fmt::format("Found archive {} that is too large", archive_path.string());
                rename_bad_file(archive_path);
                break;
            }
            case ArchiveTooLargeState::AfterProcessing:
            {
                PLOG_ERROR << fmt::format("Archive {} became too large after processing. Ignoring it.",
                                          archive_path.string());
                break;
            }
        }
        return ArchiveTooLargeAction::Skip;
    }
    [[nodiscard]] auto transform_file(btu::modmanager::ModFile file) noexcept
        -> std::optional<std::vector<std::byte>> override
    {
        const auto path   = file.relative_path;
        auto path_for_log = btu::common::as_ascii_string(path.u8string());

        PLOG_VERBOSE << fmt::format("Processing file {}", path_for_log);

        auto ret = process_file(std::move(file), settings_);

        progress_callback_(path);

        if (!ret)
        {
            if (ret.error() == k_error_no_work_required) // everything is fine
                return std::nullopt;

            PLOG_ERROR << fmt::format("Failed to process file {}: {}", path_for_log, ret.error().message());

            // TODO: rename bad files

            return std::nullopt;
        }

        return std::move(*ret);
    }
};

// TODO: handle several mods at once
void Manager::run_optimization()
{
    PLOG_INFO << fmt::format("Processing: {}", settings_.current_profile().input_path.string());

    const auto start_time = std::chrono::system_clock::now();
    PLOG_INFO << fmt::format("Beginning. Start time: {}", start_time);

    const auto bsa_sets = btu::bsa::Settings::get(settings_.current_profile().target_game);
    auto mod            = btu::modmanager::ModFolder(settings_.current_profile().input_path, bsa_sets);

    PLOG_INFO << "Counting files...";
    PLOG_INFO << fmt::format("Found {} files", mod.size());
    emit files_counted(mod.size());

    PLOG_INFO << "Parsing plugins...";
    const auto plugin_info = get_plugin_info(mod);
    apply_plugin_info(settings_, plugin_info);

    if (settings_.current_profile().bsa_operation == BsaOperation::Extract)
        unpack_directory(mod.path());

    auto transformer = ModTransformer{settings_,
                                      [this](const btu::Path &path) { emit_progress_rate_limited(path); }};
    mod.transform(transformer);

    if (settings_.current_profile().bsa_operation == BsaOperation::Create)
        pack_directory(mod.path());

    const auto end_time     = std::chrono::system_clock::now();
    const auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
    PLOG_INFO << fmt::format("Finished. End time: {}. Elapsed time: {}s", end_time, elapsed_time);
    emit end();
}
} // namespace cao
