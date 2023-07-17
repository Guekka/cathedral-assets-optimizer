/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "ProfilesManagerWindow.hpp"

#include "settings/base_types.hpp"
#include "settings/settings.hpp"
#include "ui_ProfilesManagerWindow.h"
#include "utils/utils.hpp"

#include <QInputDialog>
#include <QMessageBox>

namespace cao {
ProfilesManagerWindow::ProfilesManagerWindow(Settings &profiles_, QWidget *parent)
    : QDialog(parent)
    , profiles(profiles_)
    , ui_(std::make_unique<Ui::ProfilesManagerWindow>())
{
    ui_->setupUi(this);
    /*
    auto &games = *ui_->games;
     FIXME
    setData(games, tr("Morrowind"), btu::Game::TES3);
    setData(games, tr("Oblivion"), btu::Game::TES4);
    setData(games, tr("Skyrim LE (2011)"), btu::Game::SLE);
    setData(games, tr("Skyrim SE (2016)"), btu::Game::SSE);
    setData(games, tr("Fallout New Vegas"), btu::Game::FNV);
    setData(games, tr("Fallout 4"), btu::Game::FO4);

    connect(ui_->games, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        // auto &profile = profiles.get(ui_->profiles->currentText());
        // FIXME        profile.getGeneralSettings().eGame = ui_->games->itemData(idx).value<btu::Game>();
    });

    connect(ui_->profiles, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        selectRightGame(ui_->profiles->itemText(idx));
    });

    connect(ui_->newPushButton, &QPushButton::pressed, this, &ProfilesManagerWindow::createProfile);
    connect(ui_->removePushButton, &QPushButton::pressed, this, &ProfilesManagerWindow::deleteCurrentProfile);
    connect(ui_->renamePushButton, &QPushButton::pressed, this, &ProfilesManagerWindow::renameCurrentProfile);

    updateProfiles(*ui_->profiles);
    selectRightGame(ui_->profiles->currentText());
     */
}

ProfilesManagerWindow::~ProfilesManagerWindow() = default;

void ProfilesManagerWindow::updateProfiles(QComboBox &box)
{
    box.clear();
    /*
    box.addItems(profiles.list());
    selectText(box, profiles.currentProfileName());
     */
}

QString ProfilesManagerWindow::getSelectedProfile()
{
    //    return ui_->profiles->currentText();
    return "";
}

void ProfilesManagerWindow::setEnabledProfile(const QString &name)
{
    //    selectText(*ui_->profiles, name);
}

void ProfilesManagerWindow::setAllowedGames(const std::vector<btu::Game> &games)
{
    auto &gameBox = ui_->games;

    for (int i = 0; i < gameBox->count(); ++i)
        setItemEnabled(*gameBox, i, false);

    for (auto game : games)
    {
        // FIXME        int idx = findData(*gameBox, game);
        //        setItemEnabled(*gameBox, idx, false);
    }
}

void ProfilesManagerWindow::createProfile()
{
    bool ok = false;
    const QString &text
        = QInputDialog::getText(this, tr("New profile"), tr("Name:"), QLineEdit::Normal, "", &ok);
    if (!ok || text.isEmpty())
        return;

    //Choosing base profile

    QStringList profilesList;
    /*
    auto &profilesBox = ui_->profiles;
    for (int i = 0; i < profilesBox->count(); ++i)
        profilesList.push_back(profilesBox->itemText(i));

    const QString &baseProfile = QInputDialog::getItem(this,
                                                       tr("Base profile"),
                                                       tr("Which profile do you want to use as a base?"),
                                                       profilesList,
                                                       profilesBox->currentIndex(),
                                                       false,
                                                       &ok);

    if (!ok)
        return;

    this->profiles.create_profile(text, baseProfile);
    */
    setProfile(text);
}

void ProfilesManagerWindow::setProfile(const QString &name)
{
    //    updateProfiles(*ui_->profiles);
    //   ui_->profiles->setCurrentIndex(ui_->profiles->findText(name));
}

void ProfilesManagerWindow::deleteCurrentProfile()
{
    /*
        const QString &current = ui_->profiles->currentText();
    const auto button = QMessageBox::warning(
        this,
        tr("Remove profile"),
        tr("Are you sure you want to remove profile '%1'? This action cannot be undone").arg(current),
        QMessageBox::No | QMessageBox::Yes);

    if (button != QMessageBox::Yes)
        return;
*/
    //    if (!profiles.remove(current))
    //      QMessageBox::critical(this, tr("Removal failed"), tr("Failed to delete profile: %1").arg(current));

    // updateProfiles(*ui_->profiles);
}

void ProfilesManagerWindow::renameCurrentProfile()
{
    const QString &text = QInputDialog::getText(this, tr("Rename profile"), tr("New name:"));

    if (text.isEmpty())
        return;
    /*
    const QString &current = ui_->profiles->currentText();
    if (!profiles.rename(current, text))
        QMessageBox::critical(this, tr("Renaming failed"), tr("Failed to rename profile: %1").arg(current));

    updateProfiles(*ui_->profiles);
    */
}

void ProfilesManagerWindow::selectRightGame(const QString &profileName)
{
    /*
    if (!profiles.exists(profileName))
        return;

    auto &profile = profiles.get(profileName);
    selectData(*ui_->games, profile.getGeneralSettings().eGame());
     */
}

} // namespace cao