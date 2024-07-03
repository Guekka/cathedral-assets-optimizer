/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "settings/settings.hpp"

#include <QObject>
#include <QString>
#include <thread>

namespace cao {
/// \brief The Manager class is responsible for the optimization process.
/// It is the main class of the program and the only "backend" class to be used by the GUI.
class Manager final : public QObject
{
    Q_OBJECT

public:
    void run_optimization(Settings settings, std::stop_token stop_token);

private:
    Settings settings_;
    std::stop_token stop_token_;

    void process_single_mod(const btu::Path &path);
    void process_several_mods(const btu::Path &path);

    void unpack_directory(const std::filesystem::path &directory_path) const;
    void pack_directory(const std::filesystem::path &directory_path) const;

    void emit_progress_rate_limited(const btu::Path &path);
    // TODO: would a mutex be better?
    std::atomic<std::chrono::steady_clock::time_point> last_emission_;
    std::atomic_uint32_t files_processed_since_last_emissions_;

signals:
    void files_counted(size_t count) const;
    void files_processed(std::filesystem::path last_relative_path, size_t count_since_last) const;
    void end() const;
};
} // namespace cao
