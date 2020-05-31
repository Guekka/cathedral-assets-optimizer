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
        modules_.emplace_back(std::make_unique<T>());
        ui_->tabWidget->addTab(&*modules_.back(), name);
    }

    template<typename T>
    void removeModule()
    {
        auto it = std::find_if(modules_.begin(),
                               modules_.end(),
                               [](const std::unique_ptr<IWindowModule> &mod) {
                                   return dynamic_cast<T *>(mod);
                               });
        if (it != modules_.end())
            modules_.erase(it);
    }

    void connectAll();
    void disconnectAll();

private:
    std::unique_ptr<Ui::MainWindow> ui_;

    int progressBarValue_{};
    std::unique_ptr<Manager> caoProcess_;
    std::vector<std::unique_ptr<IWindowModule>> modules_;

    void createProfile();
    void refreshProfiles();
    void setProfile(const QString &name);

    void resetUi();
    void loadUi();

    void updateLog() const;

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
