/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "manager.hpp"

#include "Logger.hpp"
#include "Utils/Filesystem.hpp"
#include "main_process.hpp"
#include "settings/settings.hpp"

#include <btu/bsa/settings.hpp>
#include <btu/modmanager/mod_manager.hpp>
#include <fmt/format.h>

#include <QDateTime>

namespace cao {

Manager::Manager(Settings settings)
    : settings_(std::move(settings))
{
    // Preparing logging
    if (!init_logging(Settings::data_directory()))
        throw std::runtime_error("Failed to initialize logging.");
}

// TODO: use std::chrono (GCC 13.1) instead of QDateTime
void Manager::run_optimization()
{
    PLOG_INFO << "Processing: " << settings_.current_profile().input_path;

    const auto start_time = QDateTime::currentDateTime();
    PLOG_INFO << fmt::format("Beginning. Start time: {}", start_time.toString("hh'h'mm'm'").toStdString());

    auto archive_ext = btu::bsa::Settings::get(settings_.current_profile().target_game).extension;
    auto mods        = btu::modmanager::ModsFolder(settings_.current_profile().input_path, archive_ext);

    const auto file_count = flow::from(mods).count(); // should take a while, but worth it for the progress bar
    emit file_counted(file_count);

    flow::for_each(mods, [this](btu::modmanager::ModFolder &&mod) {
        mod.transform([this](btu::modmanager::ModFolder::ModFile &&file) {
            auto path = file.relative_path;
            auto ret  = process_file(std::move(file), settings_);

            if (!ret)
            {
                PLOG_ERROR << fmt::format("Failed to process file {}: {}",
                                          btu::common::as_ascii(path.u8string()),
                                          ret.error().message());
            }

            emit file_processed(path);

            return *ret;
        });
    });

    const auto end_time     = QDateTime::currentDateTime();
    const auto elapsed_time = start_time.secsTo(end_time);
    PLOG_INFO << fmt::format("Finished. End time: {}. Elapsed time: {}s",
                             end_time.toString("hh'h'mm'm'").toStdString(),
                             elapsed_time);
    emit end();
}
} // namespace cao
