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
#include <tuple>
#include <utility>
namespace cao {

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

[[nodiscard]] auto get_plugin_info(btu::modmanager::ModFolder &mod) -> PluginInfo
{
    class PluginInfoVisitor final : public btu::modmanager::ModFolderIterator
    {
    public:
        explicit PluginInfoVisitor(btu::Path mod_root)
            : mod_root_(std::move(mod_root))
        {
        }

        [[nodiscard]] auto archive_too_large(const btu::Path & /*archive_path*/,
                                             ArchiveTooLargeState /*state*/) noexcept
            -> ArchiveTooLargeAction override
        {
            return ArchiveTooLargeAction::Skip;
        }

        void process_file(const btu::modmanager::ModFile file) noexcept override
        {
            static constexpr auto k_plugin_exts = std::to_array({".esp", ".esm", ".esl"});
            if (!btu::common::contains(k_plugin_exts, file.relative_path.extension()))
                return;

            PLOG_INFO << fmt::format("Parsing plugin {}", file.relative_path.string());

            const auto absolute_path = mod_root_ / file.relative_path;
            auto headparts           = btu::esp::list_headparts(absolute_path);
            auto landscape_textures  = btu::esp::list_landscape_textures(absolute_path);

            if (!headparts && !landscape_textures)
            {
                PLOGV << fmt::format("Plugin {} has no headparts or landscape textures",
                                     file.relative_path.string());
            }

            const auto lock = std::scoped_lock(mutex_);

            if (headparts)
                result_.headparts.insert(result_.headparts.end(), headparts->begin(), headparts->end());

            if (landscape_textures)
                result_.landscape_textures.insert(result_.landscape_textures.end(),
                                                  landscape_textures->begin(),
                                                  landscape_textures->end());
        }

        [[nodiscard]] auto result() const noexcept -> PluginInfo { return result_; }

    private:
        std::mutex mutex_;
        PluginInfo result_;
        btu::Path mod_root_;
    };

    auto iterator = PluginInfoVisitor(mod.path());
    mod.iterate(iterator);

    return iterator.result();
}

[[nodiscard]] auto get_bsa_settings(const Settings &sets) noexcept -> btu::bsa::Settings
{
    return btu::bsa::Settings::get(sets.current_profile().target_game);
}

void Manager::unpack_directory(const std::filesystem::path &directory_path) const
{
    emit stage_changed(tr("Extracting BSAs"));
    const bool dry_run = settings_.current_profile().dry_run;
    PLOG_INFO << fmt::format("{}Extracting archives in {}",
                             dry_run ? "[Dry run] " : "",
                             directory_path.string());

    const auto bsa_sets = get_bsa_settings(settings_);
    const auto archives = list_archive(directory_path, bsa_sets);

    if (archives.empty())
    {
        PLOG_INFO << "No archives found";
        return;
    }

    emit files_counted(archives.size());

    std::ranges::for_each(archives, [this, dry_run, &directory_path](const btu::Path &entry) {
        if (dry_run)
        {
            // Preview only: never extract files or remove the archive.
            std::ignore = preview_unpack_archive(entry, directory_path);
            emit files_processed(entry, 1);
            return;
        }

        const auto res = unpack(btu::bsa::UnpackSettings{
            .file_path                = entry,
            .remove_arch              = true,
            .overwrite_existing_files = false,
            .extract_to_dir           = std::nullopt,
        });

        emit files_processed(entry, 1);

        if (!res)
        {
            PLOGE << "Failed to unpack archive: " << entry.string() << ". Reason: " << res.error().message();
        }
    });
}

static void apply_plugin_info(PerFileSettings &sets, const PluginInfo &info)
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

static void apply_plugin_info(Settings &sets, const PluginInfo &info)
{
    auto &profile = sets.current_profile();

    std::ranges::for_each(profile.per_file_settings(),
                          [&info](auto &per_file_sets) { apply_plugin_info(*per_file_sets, info); });
}

// TODO: think about adding files to existing BSAs
void Manager::pack_directory(const std::filesystem::path &directory_path) const
{
    emit stage_changed(tr("Creating BSAs"));
    const bool dry_run = settings_.current_profile().dry_run;
    PLOG_INFO << fmt::format("{}Packing directory {}", dry_run ? "[Dry run] " : "", directory_path.string());
    emit files_counted(0);
    emit files_processed("archive packing", 0);

    const auto bsa_sets = get_bsa_settings(settings_);

    const auto &profile = settings_.current_profile();
    const auto compress = settings_.current_profile().bsa_allow_compression ? btu::bsa::Compression::Yes
                                                                            : btu::bsa::Compression::No;

    const auto pack_settings
        = btu::bsa::PackSettings{.input_dir       = directory_path,
                                 .game_settings   = bsa_sets,
                                 .compress        = compress,
                                 .allow_file_pred = [profile](const auto &dir, const auto &file_info) {
                                     const auto relative_path = file_info.path().lexically_relative(dir);
                                     const auto pack = profile.get_per_file_settings(relative_path).pack;

                                     if (!pack && std::filesystem::is_regular_file(file_info))
                                     {
                                         PLOGV << fmt::format("Skipping file {} from packing",
                                                              relative_path.string());
                                     }

                                     return pack;
                                 }};

    if (dry_run)
    {
        if (stop_token_.stop_requested())
            return;

        std::ignore = preview_pack_directory(pack_settings, profile.bsa_remove_files);

        if (profile.bsa_make_dummy_plugins)
        {
            PLOGI
                << "Dry run: would create dummy plugins for packed archives; plugin names and count are not "
                   "predicted because archive splitting is unknown";
        }
        return;
    }

    pack(pack_settings).for_each([&](auto archive) {
        if (stop_token_.stop_requested())
            return;

        try
        {
            write_single_archive(directory_path,
                                 std::move(archive),
                                 bsa_sets,
                                 settings_.current_profile().bsa_remove_files,
                                 settings_.current_profile().bsa_make_overrides);
        }
        catch (const std::exception &e)
        {
            PLOGE << "Failed to pack archive: " << e.what();
        }
        emit files_processed("archive packing", 1);
    });

    if (profile.bsa_make_dummy_plugins)
    {
        remake_dummy_plugins(directory_path, bsa_sets);
    }
}

void Manager::emit_progress_rate_limited(const btu::Path &path)
{
    ++files_processed_since_last_emissions_;
    constexpr auto emit_every = std::chrono::milliseconds(200);

    const auto now = std::chrono::steady_clock::now();
    if (now - last_emission_.load() > emit_every)
    {
        last_emission_.store(now);
        emit files_processed(path, files_processed_since_last_emissions_);
        files_processed_since_last_emissions_ = 0;
    }
}

class ModTransformer final : public btu::modmanager::ModFolderTransformer
{
public:
    using ProgressCallback = std::function<void(const btu::Path &)>;

private:
    Settings settings_;
    std::stop_token stop_token_;
    ProgressCallback progress_callback_;

public:
    ModTransformer(Settings settings, std::stop_token stop_token, ProgressCallback progress_callback)
        : settings_(std::move(settings))
        , stop_token_(std::move(stop_token))
        , progress_callback_(std::move(progress_callback))
    {
    }

    [[nodiscard]] auto archive_too_large(const btu::Path &archive_path,
                                         ArchiveTooLargeState state) noexcept -> ArchiveTooLargeAction override
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

        auto ret = process_file(std::move(file), settings_);

        progress_callback_(path);

        if (!ret)
        {
            if (ret.error() != k_error_no_work_required)
            {
                PLOG_ERROR << fmt::format("Failed to process file {}: {}",
                                          path_for_log,
                                          ret.error().message());
            }

            // TODO: rename bad files

            return std::nullopt;
        }

        return std::move(*ret);
    }

    [[nodiscard]] auto stop_requested() const noexcept -> bool override
    {
        return stop_token_.stop_requested();
    }

    void failed_to_read_archive(const btu::Path &archive_path) noexcept override
    {
        PLOGE << fmt::format("Failed to read archive {}", archive_path.string());
        rename_bad_file(archive_path);
    }

    // TODO: think more thoroughly about these error handling functions

    void failed_to_write_transformed_file(const btu::Path &relative_path,
                                          const std::span<const std::byte> content) noexcept override
    {
        PLOGE << fmt::format("Failed to write transformed file {}. After processing, file size was {}",
                             relative_path.string(),
                             content.size());
    }

    void failed_to_read_transformed_file(const btu::Path &relative_path,
                                         const std::span<const std::byte> content) noexcept override
    {
        PLOGE << fmt::format("Failed to read transformed file {}. After processing, file size was {}",
                             relative_path.string(),
                             content.size());
    }

    void failed_to_write_archive(const btu::Path &old_archive_path,
                                 const btu::Path &new_archive_path) noexcept override
    {
        PLOGE << fmt::format("Failed to write archive {} to {}",
                             old_archive_path.string(),
                             new_archive_path.string());
    }
};

void Manager::process_single_mod(const btu::Path &path)
{
    emit stage_changed(tr("Reading mod contents"));
    const auto bsa_sets = get_bsa_settings(settings_);
    auto mod            = btu::modmanager::ModFolder(path, bsa_sets);

    const bool prepare_assets = !quick_task_ || std::holds_alternative<PrepareSkyrimAssetsTask>(*quick_task_);
    if (prepare_assets)
    {
        PLOG_INFO << "Parsing plugins...";
        const auto plugin_info = get_plugin_info(mod);
        apply_plugin_info(settings_, plugin_info);
    }

    if (stop_token_.stop_requested())
        return;

    if (quick_task_ && std::holds_alternative<ExtractBsaTask>(*quick_task_))
    {
        unpack_directory(mod.path());
        return;
    }
    if (quick_task_ && std::holds_alternative<CreateBsaTask>(*quick_task_))
    {
        pack_directory(mod.path());
        return;
    }

    if (settings_.current_profile().bsa_operation == BsaOperation::Extract)
        unpack_directory(mod.path());

    const auto size = mod.size();
    PLOG_INFO << fmt::format("Found {} files", size);

    emit files_counted(size);
    emit stage_changed(tr("Processing assets"));

    auto transformer = ModTransformer{settings_, stop_token_, [this](const btu::Path &path) {
                                          emit_progress_rate_limited(path);
                                      }};

    mod.transform(transformer);
    if (stop_token_.stop_requested())
        return;

    if (settings_.current_profile().bsa_operation == BsaOperation::Create)
        pack_directory(mod.path());
}

void Manager::process_several_mods(const btu::Path &path)
{
    emit stage_changed(tr("Finding mods in the selected folder"));
    PLOGI << "Processing several mods in " << path.string();
    switch (btu::modmanager::find_manager(path))
    {
        case btu::modmanager::ModManager::Vortex:
            PLOGI << "This directory appears to be a Vortex mod directory";
            break;
        case btu::modmanager::ModManager::MO2:
            PLOGI << "This directory appears to be a MO2 mod directory";
            break;
        case btu::modmanager::ModManager::ManualForced:
            PLOGI << "This directory appears to be managed manually and was marked for processing";
            break;
        case btu::modmanager::ModManager::None:
            PLOGI << "This directory does not appear to be a mod directory. Processing it anyway, but it may "
                     "not work as expected";
            break;
    }

    // TODO: improve handling per mod manager
    const auto mod_folders = flux::from_range(btu::fs::directory_iterator(path)).filter([](const auto &entry) {
        return btu::fs::is_directory(entry.path());
    });

    for (const auto &mod_folder : mod_folders)
    {
        if (stop_token_.stop_requested())
            return;
        process_single_mod(mod_folder);
    }
}

void Manager::run_optimization(Settings settings,
                               std::stop_token stop_token,
                               std::optional<QuickTask> quick_task)
{
    settings_   = std::move(settings);
    stop_token_ = std::move(stop_token);
    quick_task_ = quick_task;

    emit stage_changed(tr("Preparing run"));
    PLOG_INFO << fmt::format("Processing mod: {}", settings_.current_profile().input_path.string());
    if (quick_task_)
    {
        if (std::holds_alternative<PrepareSkyrimAssetsTask>(*quick_task_))
            PLOG_INFO << "Quick task: Prepare Skyrim assets";
        else if (std::holds_alternative<ExtractBsaTask>(*quick_task_))
            PLOG_INFO << "Quick task: Extract BSAs";
        else
            PLOG_INFO << "Quick task: Create BSAs";
    }

    const auto start_time = std::chrono::system_clock::now();
    PLOG_INFO << fmt::format("Beginning. Start time: {}", start_time);

    switch (settings_.current_profile().optimization_mode)
    {
        case OptimizationMode::SingleMod: process_single_mod(settings_.current_profile().input_path); break;
        case OptimizationMode::SeveralMods:
            process_several_mods(settings_.current_profile().input_path);
            break;
    }

    const auto end_time     = std::chrono::system_clock::now();
    const auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
    PLOG_INFO << fmt::format("Finished. End time: {}. Elapsed time: {}s", end_time, elapsed_time);
    const bool cancelled = stop_token_.stop_requested();
    emit stage_changed(cancelled ? tr("Stopped before all work was complete") : tr("Finishing run"));
    emit end(cancelled);
}
} // namespace cao
