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

// TODO: add export/import profiles
class ProfilesManagerWindow : public QDialog
{
    Q_DECLARE_TR_FUNCTIONS(ProfilesManagerWindow)

public:
    explicit ProfilesManagerWindow(Settings &profiles, QWidget *parent = nullptr);

    ProfilesManagerWindow(const ProfilesManagerWindow &)                     = delete;
    auto operator=(const ProfilesManagerWindow &) -> ProfilesManagerWindow & = delete;

    ProfilesManagerWindow(ProfilesManagerWindow &&)                     = delete;
    auto operator=(ProfilesManagerWindow &&) -> ProfilesManagerWindow & = delete;

    ~ProfilesManagerWindow() override; // = default

private:
    Settings &profiles_;
    std::unique_ptr<Ui::ProfilesManagerWindow> ui_;
    void create_profile();

    void delete_current_profile();

    void update_profiles(QComboBox &box);

    void import_profile();
    void export_selected_profile();
};
} // namespace cao
