/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <QDir>

#include "Logger.hpp"
#include "Utils/Algorithms.hpp"
#include "plog/Appenders/RollingFileAppender.h"
#include "plog/Formatters/TxtFormatter.h"
#include "plog/Init.h"

namespace CAO {
void initCustomLogger(const QString &logPath)
{
    //Cancelling if logger is already ready
    if (plog::get())
        return;

    //Creating log folder
    const QDir dir;
    if (!dir.mkpath(QFileInfo(logPath).path()))
        throw std::runtime_error("Cannot make log path: " + QFileInfo(logPath).path().toStdString());

    //Creating log file
    QFile file(logPath);

    if (!file.open(QFile::ReadWrite | QFile::Append))
        throw std::runtime_error("Cannot open log file: " + logPath.toStdString());

    static plog::RollingFileAppender<plog::CustomFormatter> appender(qPrintable(logPath), 250'000, 1'000);
    plog::init(plog::Severity::verbose, &appender);
}
} // namespace CAO

namespace plog {
util::nstring CustomFormatter::header()
{
    return TxtFormatter::header();
}

plog::util::nstring plog::CustomFormatter::format(const plog::Record &record)
{
    tm t{};
    util::localtime_s(&t, &record.getTime().time);

    util::nostringstream ss;
    ss
        //Time
        << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1
        << PLOG_NSTR("-") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << PLOG_NSTR(" ")
        << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << PLOG_NSTR(":")
        << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << PLOG_NSTR(":")
        << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec << PLOG_NSTR(".")
        << std::setfill(PLOG_NSTR('0')) << std::setw(3) << record.getTime().millitm << PLOG_NSTR(" ")
        << std::setfill(PLOG_NSTR(' ')) << std::setw(5)
        << std::left
        //Actual message
        << severityToString(record.getSeverity()) << PLOG_NSTR(" ") << PLOG_NSTR("[") << record.getTid()
        << PLOG_NSTR("] ") << PLOG_NSTR("[") << record.getFunc() << PLOG_NSTR("@") << record.getLine()
        << PLOG_NSTR("] ") << record.getMessage() << PLOG_NSTR('|') //Makes life easier for parsers
        << PLOG_NSTR("\n");

    return ss.str();
}
} // namespace plog
