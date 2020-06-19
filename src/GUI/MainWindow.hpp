/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "IWindowModule.hpp"
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

    Ui::MainWindow &mainUI() { return *ui_; }
    const Ui::MainWindow &mainUI() const { return *ui_; }

    template<typename T>
    void addModule(const QString &name)
    {
        T *mod = new T;
        ui_->tabWidget->insertTab(0, mod, name);
        connectModule(*mod);
    }

    template<typename T>
    void removeModule()
    {
        for (int i = 0; i < ui_->tabWidget->count(); i++)
            if (dynamic_cast<T *>(ui_->tabWidget->widget(i)))
            {
                ui_->tabWidget->removeTab(i);
                break;
            }
    }

    void refreshProfiles();

private:
    std::unique_ptr<Ui::MainWindow> ui_;
    std::unique_ptr<Manager> caoProcess_;
    QFile logFile;

    void connectModule(IWindowModule &);
    void reconnectModules();

    void connectAll();
    void disconnectAll();

    void createProfile();
    void setProfile(const QString &name);

    void resetUi();
    void loadUi();

    void updateLog();

    void initProcess();
    void endProcess();
    void readProgress(const QString &text, const int &max, const int &value);

    void setDarkTheme(const bool &enabled);
    void showTutorialWindow(const QString &title, const QString &text);

    void setAdvancedSettingsEnabled(const bool &value);

    void firstStart();

    //Qt override
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
};
} // namespace CAO
