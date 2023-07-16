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
    MainWindow() noexcept;
    ~MainWindow() override;

    void addModule(IWindowModule *module);
    void clearModules();

    void setPatternsEnabled(bool state);
    void setLevelSelectorHandler(const std::function<void()> &callback);

private:
    Settings settings_;

    std::unique_ptr<Ui::MainWindow> ui_;
    std::unique_ptr<Manager> caoProcess_;
    std::unique_ptr<ProgressWindow> progressWindow_;

    std::vector<IWindowModule *> getModules();

    using ConnectionWrapper::connect;
    using ConnectionWrapper::connectWrapper;

    void connectModule(IWindowModule &);
    void reconnectModules();
    void freezeModules(bool state = true);

    void connectThis();
    void disconnectThis();
    void reconnectThis();

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
