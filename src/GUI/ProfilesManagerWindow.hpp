/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "Settings/BaseTypes.hpp"
#include "pch.hpp"
#include "ui_ProfilesManagerWindow.h"

namespace Ui {
class ProfilesManagerWindow;
}

namespace CAO {
class Profiles;
class ProfilesManagerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ProfilesManagerWindow(Profiles &profiles, QWidget *parent = nullptr);
    QString getSelectedProfile();
    void setEnabledProfile(const QString &name);
    void setAllowedGames(const std::vector<Games> &games);

    void updateProfiles(QComboBox &box);

private:
    Profiles &profiles;
    std::unique_ptr<Ui::ProfilesManagerWindow> ui_;

    void createProfile();
    void setProfile(const QString &name);

    void deleteCurrentProfile();
    void renameCurrentProfile();

    void selectRightGame(const QString &profileName);
};
} // namespace CAO
