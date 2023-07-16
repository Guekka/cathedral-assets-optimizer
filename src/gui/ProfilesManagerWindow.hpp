/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "settings/base_types.hpp"

#include <btu/common/games.hpp>

#include <QCoreApplication>
#include <QDialog>

namespace Ui {
class ProfilesManagerWindow;
}

class QComboBox;

namespace cao {
class Settings;
class ProfilesManagerWindow : public QDialog
{
    Q_DECLARE_TR_FUNCTIONS(ProfilesManagerWindow)

public:
    explicit ProfilesManagerWindow(Settings &profiles, QWidget *parent = nullptr);
    ~ProfilesManagerWindow(); // = default

    QString getSelectedProfile();
    void setEnabledProfile(const QString &name);
    void setAllowedGames(const std::vector<btu::Game> &games);

    void updateProfiles(QComboBox &box);

private:
    Settings &profiles;
    std::unique_ptr<Ui::ProfilesManagerWindow> ui_;

    void createProfile();
    void setProfile(const QString &name);

    void deleteCurrentProfile();
    void renameCurrentProfile();

    void selectRightGame(const QString &profileName);
};
} // namespace cao
