/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "IWindowModule.hpp"
#include "ProgressWindow.hpp"
#include "settings/base_types.hpp"
#include "settings/settings.hpp"

#include <QCoreApplication>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

namespace cao {
class Manager;
class Settings;

class MainWindow final : public QMainWindow, ConnectionWrapper
{
    Q_DECLARE_TR_FUNCTIONS(MainWindow)

public:
    void set_level(GuiMode mode);

    MainWindow() noexcept;
    ~MainWindow() override;

    void add_module(std::unique_ptr<IWindowModule> mod);
    void clear_modules();

    void set_patterns_enabled(bool state);

private:
    Settings settings_;

    std::unique_ptr<Ui::MainWindow> ui_;
    std::unique_ptr<Manager> caoProcess_;
    std::unique_ptr<ProgressWindow> progressWindow_;

    using ConnectionWrapper::connect;
    using ConnectionWrapper::connectWrapper;

    void connect_module(IWindowModule &);
    void reconnect_modules();

    void connect_this();
    void disconnect_this();
    void reconnect_this();

    std::vector<IWindowModule *> get_modules();

    void resetUi();
    void loadUi();

    void initProcess();
    void endProcess();

    void showTutorialWindow(const QString &title, const QString &text);

    void firstStart();

    // Qt override
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
};
} // namespace cao
