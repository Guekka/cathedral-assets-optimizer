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
#include <QTimer>
#include <QWidget>

class QCloseEvent;
class QTextStream;

namespace Ui {
class ProgressWindow;
} // namespace Ui

Q_DECLARE_METATYPE(plog::Severity)

namespace cao {

class LogReader
{
public:
    struct LogEntry
    {
        explicit LogEntry(QString line);
        LogEntry(QString line, plog::Severity severity);

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

class ProgressWindow final : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressWindow(LogReader log_reader, QWidget *parent = nullptr);

    ProgressWindow(const ProgressWindow &)                     = delete;
    auto operator=(const ProgressWindow &) -> ProgressWindow & = delete;

    ProgressWindow(ProgressWindow &&)                     = delete;
    auto operator=(ProgressWindow &&) -> ProgressWindow & = delete;

    ~ProgressWindow() override; // = default

    void set_maximum(int max);
    void step(std::optional<QString> text = std::nullopt);
    void end();

signals:
    void cancelled();

protected:
    [[maybe_unused]] void closeEvent(QCloseEvent *event) override;

private:
    int current_value_{0};
    std::optional<QString> last_text_;
    QTimer timer_;

    LogReader log_reader_;
    std::unique_ptr<Ui::ProgressWindow> ui_;

    void update_all();

    void update_progress_bar(const QString &text, int max, int value);
    void update_log(plog::Severity log_severity);
};
} // namespace cao
