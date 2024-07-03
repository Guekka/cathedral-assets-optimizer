/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "IWindowModule.hpp"
#include "ModuleDisplay.hpp"
#include "ProgressWindow.hpp"
#include "settings/settings.hpp"

#include <QCoreApplication>
#include <QMainWindow>
#include <future>

namespace Ui {
class MainWindow;
} // namespace Ui

namespace cao {
class Manager;
class Settings;

class MainWindow final : public QMainWindow
{
    Q_DECLARE_TR_FUNCTIONS(MainWindow)

public:
    explicit MainWindow(Settings settings, QWidget *parent = nullptr);

    MainWindow(const MainWindow &) = delete;
    MainWindow(MainWindow &&)      = delete;

    auto operator=(const MainWindow &) -> MainWindow & = delete;
    auto operator=(MainWindow &&) -> MainWindow      & = delete;

    ~MainWindow() override;

private:
    Settings settings_;

    std::unique_ptr<Ui::MainWindow> ui_;
    std::optional<std::jthread> cao_process_;
    std::unique_ptr<QProgressDialog> graceful_stop_dialog_;

    std::unique_ptr<ProgressWindow> progress_window_;
    ModuleDisplay module_display_{};

    void init_process();
    void stop_process_gracefully();

    void save_settings() noexcept;

    void run_gui_selector();

    void about() noexcept;

    // Qt override
    [[maybe_unused]] void closeEvent(QCloseEvent *event) override;
    [[maybe_unused]] void dragEnterEvent(QDragEnterEvent *e) override;
    [[maybe_unused]] void dropEvent(QDropEvent *e) override;
};
} // namespace cao
