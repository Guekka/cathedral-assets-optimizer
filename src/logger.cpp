/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "logger.hpp"

#include <fmt/chrono.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>

#include <filesystem>
#include <fstream>

namespace cao {

class CustomFormatter
{
public:
    [[maybe_unused]] [[nodiscard]] static auto header() -> plog::util::nstring;
    [[maybe_unused]] [[nodiscard]] static auto format(const plog::Record &record) -> plog::util::nstring;
};

[[nodiscard]] auto get_appender(const std::filesystem::path &log_file_path) noexcept -> plog::IAppender *
{
    constexpr size_t max_file_size = 1'000'000; // 1MB
    constexpr size_t keep_files    = 20;
    // plog requires a static appender
    static auto appender = plog::RollingFileAppender<CustomFormatter>(log_file_path.string().c_str(),
                                                                      max_file_size,
                                                                      keep_files);

    appender.rollLogFiles(); // Roll log files on every start

    return &appender;
}

[[nodiscard]] auto init_logging(const std::filesystem::path &log_directory) noexcept -> bool
{
    // Cancelling if logger is already ready
    if (plog::get() != nullptr)
        return true;

    // Creating log folder
    if (!std::filesystem::exists(log_directory))
        std::filesystem::create_directories(log_directory);

    // Creating log file
    const auto log_file_path = log_directory / k_log_file_name;
    std::ofstream file(log_file_path, std::ios::app);
    if (!file.is_open())
        return false;

    plog::init(plog::Severity::verbose, get_appender(log_file_path));

    PLOGV << fmt::format("Logging initialized at time: {}", std::chrono::system_clock::now());
    return true;
}

[[maybe_unused]] auto CustomFormatter::header() -> plog::util::nstring
{
    return plog::TxtFormatter::header();
}

[[maybe_unused]] auto CustomFormatter::format(const plog::Record &record) -> plog::util::nstring
{
    tm t{};
    plog::util::localtime_s(&t, &record.getTime().time);

    plog::util::nostringstream ss;
    ss
        // Time
        << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1
        << PLOG_NSTR("-") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << PLOG_NSTR(" ")
        << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << PLOG_NSTR(":")
        << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << PLOG_NSTR(":")
        << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec << PLOG_NSTR(".")
        << std::setfill(PLOG_NSTR('0')) << std::setw(3) << record.getTime().millitm << PLOG_NSTR(" ")
        << std::setfill(PLOG_NSTR(' ')) << std::setw(5)
        << std::left
        // Actual message
        << severityToString(record.getSeverity()) << PLOG_NSTR(" ") << PLOG_NSTR("[") << record.getTid()
        << PLOG_NSTR("] ") << PLOG_NSTR("[") << record.getFunc() << PLOG_NSTR("@") << record.getLine()
        << PLOG_NSTR("] ") << record.getMessage() << PLOG_NSTR('|') // Makes life easier for parsers
        << PLOG_NSTR("\n");

    return ss.str();
}
} // namespace cao
