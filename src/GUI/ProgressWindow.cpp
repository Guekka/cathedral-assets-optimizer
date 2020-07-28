/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "ProgressWindow.hpp"

namespace CAO {
ProgressWindow::ProgressWindow(const QString &logFilePath, QWidget *parent)
    : QWidget(parent)
    , logFile_(logFilePath)
    , logStream_(&logFile_)
    , ui_(std::make_unique<Ui::ProgressWindow>())
{
    ui_->setupUi(this);

    connect(ui_->clearLog, &QPushButton::clicked, this, [this] {
        ui_->text->clear();
        logEntries_.clear();
    });

    connect(ui_->debugCheckbox, &QPushButton::clicked, this, &ProgressWindow::updateLog);
    connect(ui_->errorCheckbox, &QPushButton::clicked, this, &ProgressWindow::updateLog);
    connect(ui_->infoCheckbox, &QPushButton::clicked, this, &ProgressWindow::updateLog);

    ui_->text->setMaximumBlockCount(maxBlockSize_);
    logEntries_.reserve(maxBlockSize_);
}

void ProgressWindow::update(const QString &text, int max, int value)
{
    updateProgressBar(text, max, value);
    updateLog();
}

void ProgressWindow::end()
{
    ui_->progressBar->setMaximum(100);
    ui_->progressBar->setValue(100);
    ui_->progressBar->setFormat(tr("Done"));
}

void ProgressWindow::updateProgressBar(const QString &text, int max, int value)
{
    ui_->progressBar->setFormat(text);
    ui_->progressBar->setMaximum(max);
    ui_->progressBar->setValue(value);
}

void ProgressWindow::updateEntries()
{
    QString readLine;
    readLine.reserve(1000);

    while (!logStream_.atEnd())
    {
        logStream_.readLineInto(&readLine);
        while (!readLine.endsWith('|')) //Parser helper
            readLine.append(logStream_.readLine());

        addEntry(LogEntry(readLine));
    }
}

void ProgressWindow::addEntry(ProgressWindow::LogEntry &&entry)
{
    //Do not store more than 500 entries. A circular buffer would be nice here
    if (logEntries_.size() >= 500)
        logEntries_.erase(logEntries_.begin(), logEntries_.begin() + 100);

    logEntries_.emplace_back(std::move(entry));
}

auto ProgressWindow::isAllowed()
{
    return [this](const ProgressWindow::LogEntry &entry) {
        switch (entry.severity)
        {
            case plog::Severity::info: return ui_->infoCheckbox->isChecked();

            case plog::Severity::fatal:
            case plog::warning:
            case plog::Severity::error: return ui_->errorCheckbox->isChecked();

            case plog::Severity::verbose:
            case plog::Severity::debug: return ui_->debugCheckbox->isChecked();

            default: return false;
        }
        return false;
    };
}

void ProgressWindow::updateLog()
{
    updateEntries();

    ui_->text->clear();

    if (!logFile_.isOpen())
    {
        logFile_.open(QFile::ReadOnly);

        if (!logFile_.isOpen())
            ui_->text->appendPlainText(tr("Cannot open log file"));
    }

    for (const auto &entry : logEntries_ | rx::filter(isAllowed()))
        ui_->text->appendHtml(entry.formatedText);
}

ProgressWindow::LogEntry::LogEntry(const QString &line)
{
    //We know CustomFormatter is used. Time always uses the same number of chars, 24
    constexpr int timeLength = 24;
    const auto lineSeverity  = line.mid(timeLength, line.indexOf(' ', timeLength));
    severity                 = plog::severityFromString(qPrintable(lineSeverity));

    const QString &beginColor = [this] {
        switch (severity)
        {
            case plog::none: break;
            case plog::fatal: return "<font color=DarkRed>";
            case plog::error: return "<font color=Red>";
            case plog::warning: return "<font color=Orange>";
            case plog::info: return "<font color=Green>";
            case plog::debug: return "<font color=Blue>";
            case plog::verbose: return "<font color=Purple>";
        }
        return "";
    }();

    constexpr int endFontSize = 7;
    formatedText.reserve(line.size() + beginColor.size() + endFontSize); //7 for "</font>"

    formatedText = beginColor + line + QStringLiteral("</font>");
}

} // namespace CAO
