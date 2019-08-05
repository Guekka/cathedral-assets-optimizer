/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

/* Custom functions for plog */

#pragma once
#define PLOG_OMIT_PLOG_DEFINES
#include "pch.h"
#include "plog/Log.h"

namespace plog
{
class CustomDebugFormatter
{
public:
    static util::nstring header()
    {
        //For spacing
        return util::nstring(L"<style>html{line-height:1.5rem}pre{line-height:1rem}</style>");
    }

    static util::nstring format(const Record &record)
    {
        util::nostringstream ss;

        util::nstring color;

        switch (record.getSeverity())
        {
        case none: break;
        case fatal: color = L"<font color=DarkRed>"; break;
        case error: color = L"<font color=Red>"; break;
        case warning: color = L"<font color=Orange>"; break;
        case info: color = L"<font color=Green>"; break;
        case debug: color = L"<font color=Blue>"; break;
        case verbose: color = L"<font color=Purple>"; break;
        }

        tm t;
        util::localtime_s(&t, &record.getTime().time);

        ss << PLOG_NSTR("<br>")
           << color
           //Time
           << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1 << PLOG_NSTR("-")
           << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << PLOG_NSTR(" ")
           << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << PLOG_NSTR(":")
           << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0'))
           << std::setw(2) << t.tm_sec << PLOG_NSTR(".") << std::setfill(PLOG_NSTR('0')) << std::setw(3)
           << record.getTime().millitm << PLOG_NSTR(" ") << std::setfill(PLOG_NSTR(' ')) << std::setw(5)
           << std::left
           //Actual message
           << severityToString(record.getSeverity()) << PLOG_NSTR('{') << record.getFunc() << PLOG_NSTR('@')
           << record.getLine() << PLOG_NSTR("} ") << record.getMessage() << PLOG_NSTR("</font>");

        return ss.str();
    }
};

class CustomInfoFormatter
{
public:
    static util::nstring header()
    {
        //For spacing
        return util::nstring(L"<style>html{line-height:1.5rem}pre{line-height:1rem}</style>");
    }

    static util::nstring format(const Record &record)
    {
        util::nostringstream ss;

        util::nstring color;

        switch (record.getSeverity())
        {
        case none: break;
        case fatal: color = L"<font color=DarkRed>"; break;
        case error: color = L"<font color=Red>"; break;
        case warning: color = L"<font color=Orange>"; break;
        case info: color = L"<font color=Green>"; break;
        case debug: color = L"<font color=Blue>"; break;
        case verbose: color = L"<font color=Purple>"; break;
        }

        tm t;
        util::localtime_s(&t, &record.getTime().time);

        ss << PLOG_NSTR("<br>")
           << color
           //Time
           << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1 << PLOG_NSTR("-")
           << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << PLOG_NSTR(" ")
           << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << PLOG_NSTR(":")
           << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0'))
           << std::setw(2) << t.tm_sec
           << std::left
           //Actual message
           << PLOG_NSTR(" [") << severityToString(record.getSeverity()) << PLOG_NSTR("] ") << record.getMessage()
           << PLOG_NSTR("</font>");

        return ss.str();
    }
};
} // namespace plog

inline void initCustomLogger(const QString &logPath, const bool &debugLog)
{
    //Cancelling if logger is already ready
    if (plog::get())
        return;

    //Creating log folder
    const QDir dir;
    dir.mkpath(QFileInfo(logPath).path());

    //Creating log file
    QFile file(logPath);

    if (!file.open(QFile::ReadWrite | QFile::Append))
        throw std::runtime_error("Cannot open log file: " + logPath.toStdString());

    if (debugLog)
        plog::init<plog::CustomDebugFormatter>(plog::Severity::verbose, qPrintable(logPath));
    else
        plog::init<plog::CustomInfoFormatter>(plog::Severity::info, qPrintable(logPath));
}
