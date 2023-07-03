/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "ProgressWindow.hpp"

#include "ui_ProgressWindow.h"

#include <QCloseEvent>
#include <QFile>
#include <QTextStream>

namespace CAO {
ProgressWindow::ProgressWindow(const QString &logFilePath, QWidget *parent)
    : QWidget(parent)
    , logFilePath_(logFilePath)
    , ui_(std::make_unique<Ui::ProgressWindow>())
{
    ui_->setupUi(this);

    connect(ui_->clearLog, &QPushButton::clicked, this, [this] {
        ui_->text->clear();
        logEntries_.clear();
    });

    connect(ui_->debugCheckbox, &QPushButton::clicked, this, &ProgressWindow::reloadLog);
    connect(ui_->errorCheckbox, &QPushButton::clicked, this, &ProgressWindow::reloadLog);
    connect(ui_->infoCheckbox, &QPushButton::clicked, this, &ProgressWindow::reloadLog);

    ui_->text->setMaximumBlockCount(maxBlockSize_);
    logEntries_.reserve(maxBlockSize_);
}

ProgressWindow::~ProgressWindow() = default;

void ProgressWindow::update(const QString &text, int max, int value)
{
    updateProgressBar(text, max, value);
    updateLog();
}

void ProgressWindow::end()
{
    auto &progressBar = ui_->progressBar;
    progressBar->setMaximum(100);
    progressBar->setValue(100);
    progressBar->setFormat(tr("Done"));
}

void ProgressWindow::updateProgressBar(const QString &text, int max, int value)
{
    auto &progressBar = ui_->progressBar;
    progressBar->setFormat(text);
    progressBar->setMaximum(max);
    progressBar->setValue(value);
}

void ProgressWindow::updateEntries()
{
    QFile logFile(logFilePath_);
    logFile.open(QFile::ReadOnly);

    if (!logFile.isOpen())
    {
        ui_->text->appendPlainText(tr("Cannot open log file"));
        return;
    }

    QTextStream logStream(&logFile);

    QString readLine;
    readLine.reserve(1000);

    advanceToLastRead(logStream);

    while (!logStream.atEnd())
    {
        while (true) //Parser helper
        {
            readLine.append(logStream.readLine());
            if (readLine.endsWith('|'))
            {
                readLine.chop(1); //Remove '|'
                readLine.append("<br>");
                break;
            }
            readLine.append("<br>");
        }

        addEntry(LogEntry(readLine));
        readLine.clear();
    }
    updateLastRead(logStream);
}

void ProgressWindow::resetLog()
{
    ui_->text->clear();
    for (auto &entry : logEntries_)
        entry.displayed = false;
}

void ProgressWindow::updateLastRead(QTextStream &ts)
{
    logReadPos_ = ts.pos();
}

void ProgressWindow::advanceToLastRead(QTextStream &ts)
{
    if (ts.atEnd())
        return;

    const QString first = ts.readLine();

    if (first != firstLine_) //Current and previous file are not the sam
    {
        firstLine_  = std::move(first);
        logReadPos_ = 0;
        return;
    }

    ts.seek(logReadPos_);
}

void ProgressWindow::addEntry(ProgressWindow::LogEntry &&entry)
{
    //Do not store more than 500 entries. A circular buffer would be nice here
    if (logEntries_.size() >= maxBlockSize_)
        logEntries_.erase(logEntries_.begin(), logEntries_.begin() + 100);

    logEntries_.emplace_back(std::move(entry));
}

bool ProgressWindow::isAllowed(const ProgressWindow::LogEntry &entry)
{
    if (entry.displayed)
        return false;

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
};

void ProgressWindow::updateLog()
{
    updateEntries();

    for (auto &entry : logEntries_)
    {
        if (isAllowed(entry))
        {
            ui_->text->appendHtml(entry.formatedText);
            entry.displayed = true;
        }
    }
}

void ProgressWindow::reloadLog()
{
    resetLog();
    updateLog();
}

void ProgressWindow::closeEvent(QCloseEvent *event)
{
    emit closed();
    event->accept();
}

ProgressWindow::LogEntry::LogEntry(const QString &line)
{
    //We know CustomFormatter is used. Time always uses the same number of chars, 24
    constexpr int timeLength = 24;
    const int levelEnd       = line.indexOf(' ', timeLength) - timeLength;
    const auto lineSeverity  = line.mid(timeLength, levelEnd);
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
