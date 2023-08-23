/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "manager.hpp"

#include "logger.hpp"
#include "main_process.hpp"
#include "settings/settings.hpp"

#include <btu/bsa/pack.hpp>
#include <btu/bsa/plugin.hpp>
#include <btu/bsa/settings.hpp>
#include <btu/bsa/unpack.hpp>
#include <btu/common/filesystem.hpp>
#include <btu/common/string.hpp>
#include <btu/esp/functions.hpp>
#include <btu/modmanager/mod_manager.hpp>
#include <fmt/format.h>

#include <QDateTime>
#include <filesystem>
#include <mutex>

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
        [&result, &mutex](const btu::Path &loose_path) {
            static constexpr auto k_plugin_exts = std::to_array({".esp", ".esm", ".esl"});
            if (!btu::common::contains(k_plugin_exts, loose_path.extension()))
                return;

            PLOG_INFO << fmt::format("Parsing plugin {}", loose_path.string());

            auto headparts          = btu::esp::list_headparts(loose_path);
            auto landscape_textures = btu::esp::list_landscape_textures(loose_path);

            if (!headparts && !landscape_textures)
            {
                PLOG_ERROR << fmt::format("Failed to parse plugin {}", loose_path.string());
                rename_bad_file(loose_path);
                return;
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
        [](const btu::Path &, const btu::bsa::Archive &) {});

    return result;
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

void Manager::unpack_directory(const std::filesystem::path &directory_path) const
{
    PLOG_INFO << fmt::format("Extracting archives in {}", directory_path.string());

    auto bsa_sets = btu::bsa::Settings::get(settings_.current_profile().target_game);

    std::vector archives(btu::fs::directory_iterator(directory_path), btu::fs::directory_iterator{});
    erase_if(archives, [](const auto &file) {
        return !btu::common::contains(btu::bsa::k_archive_extensions, file.path().extension());
    });

    emit files_counted(archives.size());

    std::ranges::for_each(archives, [this](const auto &path) {
        btu::bsa::unpack(btu::bsa::UnpackSettings{
            .file_path                = path,
            .remove_arch              = true,
            .overwrite_existing_files = false,
            .root_opt                 = nullptr,
        });

        emit file_processed(path);
    });
}

// TODO: think about deleting packed files
void Manager::pack_directory(const std::filesystem::path &directory_path) const
{
    PLOG_INFO << fmt::format("Packing directory {}", directory_path.string());

    auto bsa_sets = btu::bsa::Settings::get(settings_.current_profile().target_game);

    auto dir_it  = std::filesystem::directory_iterator(directory_path);
    auto plugins = btu::bsa::list_plugins(dir_it, {}, bsa_sets);

    btu::bsa::pack(btu::bsa::PackSettings{
                       .input_dir     = directory_path,
                       .game_settings = bsa_sets,
                       .compress      = btu::bsa::Compression::Yes,
                   })
        .for_each([&plugins, &bsa_sets](btu::bsa::Archive &&archive) {
            const btu::bsa::FilePath name = btu::bsa::find_archive_name(plugins, bsa_sets, archive.type());
            std::move(archive).write(name.full_path());
        });
}

// TODO: use std::chrono (GCC 13.1) instead of QDateTime
void Manager::run_optimization()
{
    PLOG_INFO << "Processing: " << settings_.current_profile().input_path;

    const auto start_time = QDateTime::currentDateTime();
    PLOG_INFO << fmt::format("Beginning. Start time: {}", start_time.toString("hh'h'mm'm'").toStdString());

    auto bsa_sets = btu::bsa::Settings::get(settings_.current_profile().target_game);
    auto mod      = btu::modmanager::ModFolder(settings_.current_profile().input_path, bsa_sets);

    PLOG_INFO << "Counting files...";
    PLOG_INFO << fmt::format("Found {} files", mod.size());
    emit files_counted(mod.size());

    PLOG_INFO << "Parsing plugins...";
    auto plugin_info = get_plugin_info(mod);
    apply_plugin_info(settings_, plugin_info);

    if (settings_.current_profile().bsa_operation == BsaOperation::Extract)
        unpack_directory(mod.path());

    mod.transform(
        [this](btu::modmanager::ModFolder::ModFile &&file) -> std::optional<std::vector<std::byte>> {
            auto path         = file.relative_path;
            auto path_for_log = btu::common::as_ascii_string(path.u8string());

            PLOG_VERBOSE << fmt::format("Processing file {}", path_for_log);

            auto ret = process_file(std::move(file), settings_);

            emit file_processed(path);

            if (!ret)
            {
                if (ret.error() == k_error_no_work_required) // everything is fine
                    return std::nullopt;

                PLOG_ERROR << fmt::format("Failed to process file {}: {}",
                                          path_for_log,
                                          ret.error().message());

                // TODO: rename bad files

                return std::nullopt;
            }

            return std::move(*ret);
        },
        [](const btu::Path &archive_path, btu::modmanager::ModFolder::ArchiveTooLargeState state) {
            switch (state)
            {
                case btu::modmanager::ModFolder::ArchiveTooLargeState::BeforeProcessing:
                {
                    // FIXME: this is likely broken as the archive is still open when this is called
                    PLOG_ERROR << fmt::format("Found archive {} that is too large", archive_path.string());
                    rename_bad_file(archive_path);
                    break;
                }
                case btu::modmanager::ModFolder::ArchiveTooLargeState::AfterProcessing:
                {
                    PLOG_ERROR << fmt::format("Archive {} became too large after processing. Ignoring it.",
                                              archive_path.string());
                    break;
                }
            }
            return btu::modmanager::ModFolder::ArchiveTooLargeAction::Skip;
        });

    if (settings_.current_profile().bsa_operation == BsaOperation::Create)
        pack_directory(mod.path());

    const auto end_time     = QDateTime::currentDateTime();
    const auto elapsed_time = start_time.secsTo(end_time);
    PLOG_INFO << fmt::format("Finished. End time: {}. Elapsed time: {}s",
                             end_time.toString("hh'h'mm'm'").toStdString(),
                             elapsed_time);
    emit end();
}
} // namespace cao
