/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "IWindowModule.hpp"
#include "ProgressWindow.hpp"
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

    template<typename T>
    void addModule(const QString &name)
    {
        T *mod = new T;
        ui_->tabWidget->insertTab(0, mod, name);
        ui_->tabWidget->setCurrentIndex(0);
        connectModule(*mod);
    }

    template<typename T>
    void removeModule()
    {
        auto& tabWidget = ui_->tabWidget;
        for (int i = 0; i < tabWidget->count(); i++)
            if (dynamic_cast<T *>(tabWidget->widget(i)))
            {
                tabWidget->removeTab(i);
                break;
            }
    }

    void refreshProfiles();

private:
    std::unique_ptr<Ui::MainWindow> ui_;
    std::unique_ptr<Manager> caoProcess_;
    std::unique_ptr<ProgressWindow> progressWindow_;

    QFile logFile;

    std::vector<IWindowModule *> getModules();
    void connectModule(IWindowModule &);
    void reconnectModules();
    void freezeModules(bool state = true);

    void connectAll();
    void disconnectAll();

    void createProfile();
    void setProfile(const QString &name);

    void resetUi();
    void loadUi();

    void initProcess();
    void endProcess();

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
