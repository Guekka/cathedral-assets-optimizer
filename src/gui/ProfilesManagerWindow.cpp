/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "ProfilesManagerWindow.hpp"

#include "settings/json.hpp"
#include "settings/settings.hpp"
#include "ui_ProfilesManagerWindow.h"
#include "utils/utils.hpp"

#include <flux.hpp>

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
namespace cao {
ProfilesManagerWindow::ProfilesManagerWindow(Settings &profiles, QWidget *parent)
    : QDialog(parent)
    , profiles_(profiles)
    , ui_(std::make_unique<Ui::ProfilesManagerWindow>())
{
    ui_->setupUi(this);

    auto &games = *ui_->games;
    set_data(games, "Morrowind", btu::Game::TES3);
    set_data(games, "Oblivion", btu::Game::TES4);
    set_data(games, "Skyrim LE (2011)", btu::Game::SLE);
    set_data(games, "Skyrim SE (2016)", btu::Game::SSE);
    set_data(games, "Fallout New Vegas", btu::Game::FNV);
    set_data(games, "Fallout 4", btu::Game::FO4);

    connect(ui_->games, &QComboBox::currentIndexChanged, this, [this] {
        profiles_.current_profile().target_game = ui_->games->currentData().value<btu::Game>();
    });

    connect(ui_->profiles, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        bool success = profiles_.set_current_profile(to_u8string(text));
        assert(success);
        success = select_data(*ui_->games, profiles_.current_profile().target_game);
        assert(success);
    });

    connect(ui_->newPushButton, &QPushButton::pressed, this, &ProfilesManagerWindow::create_profile);

    connect(ui_->removePushButton,
            &QPushButton::pressed,
            this,
            &ProfilesManagerWindow::delete_current_profile);

    connect(ui_->importPushButton, &QPushButton::pressed, this, &ProfilesManagerWindow::import_profile);
    connect(ui_->exportPushButton, &QPushButton::pressed, this, &ProfilesManagerWindow::export_selected_profile);

    update_profiles(*ui_->profiles);
    const bool success = select_data(*ui_->games, profiles_.current_profile().target_game);
    assert(success);
}

ProfilesManagerWindow::~ProfilesManagerWindow() = default;

void ProfilesManagerWindow::update_profiles(QComboBox &box)
{
    box.clear();

    const auto profiles = flux::from(profiles_.list_profiles())
                              .map([](const auto &p) { return QString::fromUtf8(p.data(), p.size()); })
                              .to<QList>();

    box.addItems(profiles);
    const bool success = select_text(box, to_qstring(profiles_.current_profile_name()));
    assert(success);
}

void ProfilesManagerWindow::create_profile()
{
    bool ok = false;
    const QString &text
        = QInputDialog::getText(this, tr("New profile"), tr("Name:"), QLineEdit::Normal, "", &ok);
    if (!ok || text.isEmpty())
        return;

    // Choosing base profile

    QStringList profiles_list;
    const auto &profiles_box = ui_->profiles;
    for (int i = 0; i < profiles_box->count(); ++i)
        profiles_list.push_back(profiles_box->itemText(i));

    const QString base_profile_text = QInputDialog::getItem(this,
                                                            tr("Base profile"),
                                                            tr("Which profile do you want to use as a base?"),
                                                            profiles_list,
                                                            profiles_box->currentIndex(),
                                                            false,
                                                            &ok);

    if (!ok)
        return;

    // should be safe to dereference, since we just checked that it exists
    auto base_profile = profiles_.get_profile(to_u8string(base_profile_text)).value();

    this->profiles_.create_profile(to_u8string(text), std::move(base_profile));
    const bool success = profiles_.set_current_profile(to_u8string(text));
    assert(success);
}

void ProfilesManagerWindow::delete_current_profile()

{
    const QString &current = ui_->profiles->currentText();
    const auto button      = QMessageBox::warning(
        this,
        tr("Remove profile"),
        tr("Are you sure you want to remove profile '%1'? This action cannot be undone").arg(current),
        QMessageBox::No | QMessageBox::Yes);

    if (button != QMessageBox::Yes)
        return;

    profiles_.remove(to_u8string(current));
    update_profiles(*ui_->profiles);
}

void ProfilesManagerWindow::import_profile()
{
    const auto raw_path = QFileDialog::getOpenFileName(this,
                                                       tr("Import profile"),
                                                       QString(),
                                                       tr("Profiles (*.json)"));

    const auto path = btu::Path(to_u8string(raw_path));

    auto profile = json::read_from_file<Profile>(path);
    if (!profile)
    {
        QMessageBox::critical(this, tr("Error"), tr("Failed to load profile"));
        return;
    }

    const auto name = path.filename().u8string();
    profiles_.create_profile(name, *std::move(profile));
}

void ProfilesManagerWindow::export_selected_profile()
{
    const auto path = QFileDialog::getSaveFileName(this,
                                                   tr("Export profile"),
                                                   QString(),
                                                   tr("Profiles (*.json)"));
    if (path.isEmpty())
        return;

    const auto &current = ui_->profiles->currentText();
    const auto& profile = profiles_.get_profile(to_u8string(current)).value();

    if (!json::save_to_file(profile, to_u8string(path)))
        QMessageBox::critical(this, tr("Error"), tr("Failed to save profile"));
}

} // namespace cao
