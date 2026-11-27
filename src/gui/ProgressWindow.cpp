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
#include <QDesktopServices>
#include <QFile>
#include <QTextStream>
#include <utility>

namespace cao {

constexpr int k_max_log_entries = 5000;

LogReader::LogReader(btu::Path log_file_path)
    : log_file_path_(BTU_MOV(log_file_path))
{
    QFile log_file(log_file_path_);
    if (!log_file.open(QFile::ReadOnly))
        return;

    auto log_stream = QTextStream(&log_file);
    if (!log_stream.atEnd())
        current_file_first_line_ = log_stream.readLine();
    while (!log_stream.atEnd())
        log_stream.readLine();

    log_read_pos_ = log_stream.pos();
}

auto LogReader::update() -> std::vector<LogEntry>
{
    // We have to reopen the file every time because it can be rotated
    QFile log_file(log_file_path_);

    if (!log_file.open(QFile::ReadOnly))
        return {{QString("Failed to open log file: %1").arg(QString::fromStdString(log_file_path_.string())),
                 plog::error}};

    auto log_stream = QTextStream(&log_file);
    advance_to_last_read(log_stream);

    auto entries = std::vector<LogEntry>{};

    while (!log_stream.atEnd())
        entries.emplace_back(read_line(log_stream));

    update_last_read(log_stream);

    return entries;
}

auto LogReader::get_log_path() const noexcept -> btu::Path
{
    return log_file_path_;
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
    const auto level_end      = text.indexOf(' ', time_length) - time_length;
    const auto line_severity  = text.mid(time_length, level_end);

    severity = plog::severityFromString(qPrintable(line_severity));
}
LogReader::LogEntry::LogEntry(QString line, plog::Severity severity)
    : text(BTU_MOV(line))
    , severity(severity)
{
}

ProgressWindow::ProgressWindow(LogReader log_reader, QWidget *parent)
    : QWidget(parent)
    , log_reader_(BTU_MOV(log_reader))
    , ui_(std::make_unique<Ui::ProgressWindow>())
{
    ui_->setupUi(this);
    setWindowModality(Qt::WindowModal); // Prevents user from interacting with the main window

    ui_->log->setWordWrap(true);

    set_data(*ui_->logLevel, "Verbose", plog::verbose);
    set_data(*ui_->logLevel, "Info", plog::info);
    set_data(*ui_->logLevel, "Error", plog::error);

    [[maybe_unused]] const bool res = select_data(*ui_->logLevel, plog::info); // Default to info
    assert(res);

    connect(ui_->clearLog, &QPushButton::clicked, this, [this] { ui_->log->clear(); });

    connect(ui_->openLogFile, &QPushButton::clicked, this, [this] {
        const auto path = to_qstring(btu::fs::absolute(log_reader_.get_log_path()).u8string());
        QDesktopServices::openUrl(QUrl("file:///" + path));
    });

    connect(ui_->logLevel, &QComboBox::currentIndexChanged, this, [this](int index) {
        update_log(ui_->logLevel->itemData(index).value<plog::Severity>());
    });

    timer_.setSingleShot(false);
    timer_.start(1000);
    connect(&timer_, &QTimer::timeout, this, &ProgressWindow::update_all);
}

ProgressWindow::~ProgressWindow() = default;

void ProgressWindow::set_run_context(const QString &context, const bool dry_run)
{
    dry_run_ = dry_run;
    ui_->runContextLabel->setText(context);
}

void ProgressWindow::set_stage(const QString &stage)
{
    if (!finished_)
    {
        current_stage_value_ = 0;
        last_text_.reset();
        ui_->progressBar->setValue(0);
        ui_->progressBar->setFormat(QStringLiteral("%p%"));
        ui_->stageLabel->setText(stage);
    }
}

void ProgressWindow::set_maximum(int max)
{
    current_stage_value_ = 0;
    ui_->progressBar->setValue(0);
    ui_->progressBar->setMaximum(max);
}

void ProgressWindow::step(std::optional<QString> text, int n)
{
    current_stage_value_ += n;
    work_units_reported_ += n;
    last_text_ = std::move(text);
}

void ProgressWindow::finish(const bool cancelled)
{
    if (finished_)
        return;

    finished_ = true;
    const auto &progress_bar = ui_->progressBar;

    if (progress_bar->maximum() == 0)
        progress_bar->setMaximum(1); // avoid busy indicator

    update_all();
    if (error_count_ > 0)
        assert(select_data(*ui_->logLevel, plog::error));

    progress_bar->setValue(progress_bar->maximum());
    progress_bar->setFormat(tr("Done"));
    const auto completion = cancelled ? tr("Stopped before all work was complete.")
                                      : dry_run_ ? tr("Dry run complete; no files were modified.")
                                                 : tr("Run complete.");
    const auto next_step = cancelled ? tr("Review the log to see what finished before stopping.")
                                     : dry_run_ ? tr("Review the planned actions, then choose Run when ready.")
                                                : error_count_ > 0
                                                   ? tr("Review the errors before using the output.")
                                                   : tr("Review the output, then test the mod in game.");
    ui_->stageLabel->setText(
        tr("%1 %2 progress items reported; %3 skip notices, %4 warnings, %5 errors. %6")
            .arg(completion)
            .arg(work_units_reported_)
            .arg(skip_notice_count_)
            .arg(warning_count_)
            .arg(error_count_)
            .arg(next_step));

    timer_.stop();
}

void ProgressWindow::update_all()
{
    if (last_text_)
        update_progress_bar(*last_text_, ui_->progressBar->maximum(), current_stage_value_);
    else
        ui_->progressBar->setValue(current_stage_value_);

    update_log(ui_->logLevel->itemData(ui_->logLevel->currentIndex()).value<plog::Severity>());
}

void ProgressWindow::update_progress_bar(const QString &text, int max, int value)
{
    const auto &progress_bar = ui_->progressBar;
    progress_bar->setFormat(text + " %p%");
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
        if (entry.text.contains(QStringLiteral("Skipping file"), Qt::CaseInsensitive)
            || entry.text.contains(QStringLiteral("would skip"), Qt::CaseInsensitive)
            || entry.text.contains(QStringLiteral("would be skipped"), Qt::CaseInsensitive))
            ++skip_notice_count_;
        if (entry.severity == plog::warning)
            ++warning_count_;
        else if (entry.severity == plog::error || entry.severity == plog::fatal)
            ++error_count_;

        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
        auto *item = new QListWidgetItem(entry.text);
        item->setForeground(QBrush(QColor(log_color(entry.severity))));
        item->setData(Qt::UserRole, QVariant::fromValue(entry.severity));
        ui_->log->addItem(item);
    }

    for (int i = 0; i < ui_->log->count(); ++i)
    {
        auto *item      = ui_->log->item(i);
        const bool hide = item->data(Qt::UserRole).value<plog::Severity>() > log_severity;
        item->setHidden(hide);
    }

    if (ui_->log->count() > k_max_log_entries)
    {
        auto *model = ui_->log->model();
        model->removeRows(0, k_max_log_entries / 2);
    }

    ui_->log->scrollToBottom();
}

[[maybe_unused]] void ProgressWindow::closeEvent(QCloseEvent *event)
{
    emit cancelled();
    event->accept();
}

} // namespace cao
