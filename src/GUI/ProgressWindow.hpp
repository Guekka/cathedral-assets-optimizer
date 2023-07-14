/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "plog/Severity.h"

#include <QWidget>

class QCloseEvent;
class QTextStream;

namespace Ui {
class ProgressWindow;
}

namespace cao {

class ProgressWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressWindow(const QString &logFilePath, QWidget *parent = nullptr);
    ~ProgressWindow(); // = default

    void update(const QString &text, int max, int value);
    void end();

signals:
    void closed();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    static constexpr size_t maxBlockSize_ = 500;

    struct LogEntry
    {
        LogEntry(const QString &line);
        QString formatedText;
        plog::Severity severity;
        bool displayed = false;
    };

    std::vector<LogEntry> logEntries_;

    std::unique_ptr<Ui::ProgressWindow> ui_;

    int64_t logReadPos_;
    QString firstLine_;

    const QString logFilePath_;

private:
    void updateProgressBar(const QString &text, int max, int value);
    void updateLog();

    void reloadLog();
    void updateEntries();
    void resetLog();

    void updateLastRead(QTextStream &ts);
    void advanceToLastRead(QTextStream &ts);

    void addEntry(LogEntry &&entry);
    bool isAllowed(const ProgressWindow::LogEntry &entry);
};
} // namespace cao
