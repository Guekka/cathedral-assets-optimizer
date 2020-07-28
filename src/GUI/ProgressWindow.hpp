/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "pch.hpp"
#include "ui_ProgressWindow.h"

namespace Ui {
class ProgressWindow;
}

namespace CAO {

class ProgressWindow : public QWidget
{
public:
    explicit ProgressWindow(const QString &logFilePath, QWidget *parent = nullptr);

    void update(const QString &text, int max, int value);
    void end();

private:
    static constexpr size_t maxBlockSize_ = 500;

    struct LogEntry
    {
        LogEntry(const QString &line);
        QString formatedText;
        plog::Severity severity;
        bool displayed = false;
    };

    QFile logFile_;
    QTextStream logStream_;

    std::vector<LogEntry> logEntries_;

    std::unique_ptr<Ui::ProgressWindow> ui_;

    void updateProgressBar(const QString &text, int max, int value);
    void updateLog();

    void reloadLog();
    void updateEntries();
    void resetLog();

    void addEntry(LogEntry &&entry);
    bool isAllowed(const ProgressWindow::LogEntry &entry);
};
} // namespace CAO
