/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "manager.hpp"

#include "logger.hpp"
#include "main_process.hpp"
#include "settings/settings.hpp"

#include <btu/bsa/settings.hpp>
#include <btu/common/filesystem.hpp>
#include <btu/common/string.hpp>
#include <btu/modmanager/mod_manager.hpp>
#include <fmt/format.h>

#include <QDateTime>
#include <filesystem>

namespace cao {

Manager::Manager(Settings settings)
    : settings_(std::move(settings))
{
}

// TODO: use std::chrono (GCC 13.1) instead of QDateTime
void Manager::run_optimization()
{
    PLOG_INFO << "Processing: " << settings_.current_profile().input_path;

    const auto start_time = QDateTime::currentDateTime();
    PLOG_INFO << fmt::format("Beginning. Start time: {}", start_time.toString("hh'h'mm'm'").toStdString());

    auto archive_ext = btu::bsa::Settings::get(settings_.current_profile().target_game).extension;
    auto mod         = btu::modmanager::ModFolder(settings_.current_profile().input_path, archive_ext);

    PLOG_INFO << "Counting files...";

    PLOG_INFO << fmt::format("Found {} files", mod.size());

    emit files_counted(mod.size());

    mod.transform([this](btu::modmanager::ModFolder::ModFile &&file) -> std::optional<std::vector<std::byte>> {
        auto path         = file.relative_path;
        auto path_for_log = btu::common::as_ascii_string(path.u8string());

        auto dbg_file_orig_size    = file.content.size();
        auto dbg_file_orig_content = file.content;
        PLOG_INFO << fmt::format("Processing file {}", path_for_log);

        auto ret = process_file(std::move(file), settings_);

        emit file_processed(path);

        if (!ret)
        {
            if (ret.error() == k_error_no_work_required) // everything is fine
                return std::nullopt;

            PLOG_ERROR << fmt::format("Failed to process file {}: {}", path_for_log, ret.error().message());
            return std::nullopt;
        }

        auto dbg_file_new_size = ret->size();
        auto dbg_file_ratio    = static_cast<double>(dbg_file_new_size)
                              / static_cast<double>(dbg_file_orig_size);

        PLOGE_IF(dbg_file_ratio > 9) << fmt::format("File {} was enlarged by {:.2f}%: {} -> {}",
                                                    path_for_log,
                                                    (dbg_file_ratio - 1.0) * 100.0,
                                                    dbg_file_orig_size,
                                                    dbg_file_new_size);

        if (dbg_file_ratio > 9)
        {
            PLOGE << "Wrote file to disk for debugging purposes: " << path_for_log << "to "
                  << btu::fs::absolute(path.filename()).string();
            btu::common::write_file(path.filename(), ret.value());
            btu::common::write_file(path.filename().replace_extension(".orig" + path.extension().string()),
                                    dbg_file_orig_content);
        }

        return std::move(*ret);
    });

    const auto end_time     = QDateTime::currentDateTime();
    const auto elapsed_time = start_time.secsTo(end_time);
    PLOG_INFO << fmt::format("Finished. End time: {}. Elapsed time: {}s",
                             end_time.toString("hh'h'mm'm'").toStdString(),
                             elapsed_time);
    emit end();
}
} // namespace cao
