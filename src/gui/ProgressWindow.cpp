/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "ProgressWindow.hpp"

#include "ui_ProgressWindow.h"
#include "utils/utils.hpp"

#include <btu/common/algorithms.hpp>
#include <btu/common/metaprogramming.hpp>

#include <QCloseEvent>
#include <QFile>
#include <QTextStream>
#include <utility>

namespace cao {

LogReader::LogReader(btu::Path log_file_path)
    : log_file_path_(BTU_MOV(log_file_path))
{
}

auto LogReader::update() -> std::vector<LogEntry>
{
    // We have to reopen the file every time because it can be rotated
    QFile log_file(log_file_path_);
    log_file.open(QFile::ReadOnly);

    if (!log_file.isOpen())
        return {};

    auto log_stream = QTextStream(&log_file);
    advance_to_last_read(log_stream);

    auto entries = std::vector<LogEntry>{};

    while (!log_stream.atEnd())
        entries.emplace_back(read_line(log_stream));

    update_last_read(log_stream);

    return entries;
}

auto LogReader::read_line(QTextStream &log_stream) -> LogEntry
{
    constexpr char log_line_end = '|';

    QString log_line = log_stream.readLine();
    log_line.reserve(1000);

    while (true)
    {
        if (log_stream.atEnd())
            break;

        if (log_line.endsWith(log_line_end))
        {
            log_line.chop(1); // Remove `log_line_end`
            break;
        }

        log_line.append(log_stream.readLine());
    }
    return LogEntry(BTU_MOV(log_line));
}

void LogReader::update_last_read(QTextStream &ts)
{
    log_read_pos_ = ts.pos();
}

void LogReader::advance_to_last_read(QTextStream &log_stream)
{
    auto first = log_stream.readLine();

    if (first != current_file_first_line_) // Current and previous file are not the same
    {
        current_file_first_line_ = std::move(first);
        log_read_pos_            = 0;
    }

    log_stream.seek(log_read_pos_);
}

LogReader::LogEntry::LogEntry(QString line)
    : text(BTU_MOV(line))
{
    // We know our CustomFormatter is used. Time always uses the same number of chars, 24
    constexpr int time_length = 24;
    const auto level_end      = line.indexOf(' ', time_length) - time_length;
    const auto line_severity  = line.mid(time_length, level_end);

    severity = plog::severityFromString(qPrintable(line_severity));
}

ProgressWindow::ProgressWindow(LogReader log_reader, QWidget *parent)
    : QWidget(parent)
    , log_reader_(BTU_MOV(log_reader))
    , ui_(std::make_unique<Ui::ProgressWindow>())
{
    ui_->setupUi(this);
    setWindowModality(Qt::WindowModal); // Prevents user from interacting with the main window

    set_data(*ui_->logLevel, "Verbose", plog::verbose);
    set_data(*ui_->logLevel, "Info", plog::info);
    set_data(*ui_->logLevel, "Error", plog::error);

    connect(ui_->clearLog, &QPushButton::clicked, this, [this] { ui_->log->clear(); });

    connect(ui_->logLevel, &QComboBox::currentIndexChanged, this, [this](int index) {
        update_log(ui_->logLevel->itemData(index).value<plog::Severity>());
    });
}

ProgressWindow::~ProgressWindow() = default;

void ProgressWindow::update(const QString &text, int max, int value)
{
    update_progress_bar(text, max, value);
    update_log(ui_->logLevel->currentData().value<plog::Severity>());
}

void ProgressWindow::end()
{
    auto &progress_bar = ui_->progressBar;
    progress_bar->setValue(progress_bar->maximum());
    progress_bar->setFormat(tr("Done"));
}

void ProgressWindow::update_progress_bar(const QString &text, int max, int value)
{
    auto &progress_bar = ui_->progressBar;
    progress_bar->setFormat(text);
    progress_bar->setMaximum(max);
    progress_bar->setValue(value);
}

[[nodiscard]] auto log_color(plog::Severity severity) noexcept -> const char *
{
    switch (severity)
    {
        case plog::none: break;
        case plog::fatal: return "DarkRed";
        case plog::error: return "Red";
        case plog::warning: return "Orange";
        case plog::info: return "Green";
        case plog::debug: return "Blue";
        case plog::verbose: return "Purple";
    }
    return "Black";
}

void ProgressWindow::update_log(plog::Severity log_severity)
{
    for (const auto &entry : log_reader_.update())
    {
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
        auto *item = new QListWidgetItem(entry.text);
        item->setForeground(QBrush(QColor(log_color(entry.severity))));
        item->setData(Qt::UserRole, QVariant::fromValue(entry.severity));
        ui_->log->addItem(item);
    }

    for (int i = 0; i < ui_->log->count(); ++i)
    {
        auto *item = ui_->log->item(i);
        item->setHidden(item->data(Qt::UserRole).value<plog::Severity>() < log_severity);
    }

    constexpr int max_log_entries = 2000;
    if (ui_->log->count() > max_log_entries)
    {
        auto *model = ui_->log->model();
        model->removeRows(0, ui_->log->count() - max_log_entries);
    }
}

void ProgressWindow::closeEvent(QCloseEvent *event)
{
    emit cancelled();
    event->accept();
}

} // namespace cao
