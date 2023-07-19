/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "plog/Severity.h"

#include <btu/common/path.hpp>

#include <QFile>
#include <QMetaType>
#include <QProgressDialog>
#include <QWidget>

class QCloseEvent;
class QTextStream;

namespace Ui {
class ProgressWindow;
}

Q_DECLARE_METATYPE(plog::Severity)

namespace cao {

class LogReader
{
public:
    struct LogEntry
    {
        explicit LogEntry(QString line);

        QString text;
        plog::Severity severity;
    };

    explicit LogReader(btu::Path log_file_path);

    [[nodiscard]] auto update() -> std::vector<LogEntry>;

private:
    void advance_to_last_read(QTextStream &log_stream);
    void update_last_read(QTextStream &ts);

    [[nodiscard]] static auto read_line(QTextStream &log_stream) -> LogEntry;

    QString current_file_first_line_;
    std::streampos log_read_pos_;

    btu::Path log_file_path_;
};

class ProgressWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressWindow(LogReader log_reader, QWidget *parent = nullptr);
    ~ProgressWindow() override; // = default

    void update(const QString &text, int max, int value);
    void end();

signals:
    void cancelled();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    LogReader log_reader_;

    void update_progress_bar(const QString &text, int max, int value);
    void update_log(plog::Severity log_severity);

    std::unique_ptr<Ui::ProgressWindow> ui_;
};
} // namespace cao
