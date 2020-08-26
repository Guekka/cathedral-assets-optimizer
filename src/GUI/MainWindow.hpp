/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "IWindowModule.hpp"
#include "ProgressWindow.hpp"
#include "Settings/BaseTypes.hpp"
#include "pch.hpp"
#include "ui_mainWindow.h"

namespace Ui {
class MainWindow;
}

namespace CAO {
class Manager;
class GeneralSettings;
class Profile;
class Profiles;

class MainWindow final : public QMainWindow
{
    Q_DECLARE_TR_FUNCTIONS(MainWindow)

public:
    MainWindow();

    void addModule(IWindowModule *module);
    void clearModules();

    void setPatternsEnabled(bool state);
    void setLevelSelectorHandler(const std::function<void()> &callback);

    void initSettings();

private:
    std::unique_ptr<Ui::MainWindow> ui_;
    std::unique_ptr<Manager> caoProcess_;
    std::unique_ptr<ProgressWindow> progressWindow_;

    std::vector<QMetaObject::Connection> connections_;

    template<typename... Args>
    void connect(Args &&... args);

    template<typename... Args>
    void connectWrapper(Args &&... args);

    std::vector<IWindowModule *> getModules();
    void connectModule(IWindowModule &);
    void reconnectModules();
    void freezeModules(bool state = true);

    void connectAll();
    void disconnectAll();
    void reconnectAll();

    void updateProfiles();
    void updatePatterns();

    void resetUi();
    void loadUi();

    void initProcess();
    void endProcess();

    void showTutorialWindow(const QString &title, const QString &text);

    void firstStart();

    //Qt override
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
};
} // namespace CAO
