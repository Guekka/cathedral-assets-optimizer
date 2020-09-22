/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "ProfilesManagerWindow.hpp"
#include "Settings/BaseTypes.hpp"
#include "Settings/Profiles.hpp"
#include "Utils.hpp"

namespace CAO {
ProfilesManagerWindow::ProfilesManagerWindow(Profiles &profiles_, QWidget *parent)
    : QDialog(parent)
    , profiles(profiles_)
    , ui_(std::make_unique<Ui::ProfilesManagerWindow>())
{
    ui_->setupUi(this);

    auto &games = *ui_->games;
    setData(games, "Morrowind", Games::Morrowind);
    setData(games, "Oblivion", Games::Oblivion);
    setData(games, "Skyrim LE (2011)", Games::SkyrimLE);
    setData(games, "Skyrim SE (2016)", Games::SkyrimSE);
    setData(games, "Fallout 3", Games::Fallout3);
    setData(games, "Fallout New Vegas", Games::FalloutNewVegas);
    setData(games, "Fallout 4", Games::Fallout4);

    connect(ui_->games, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        auto &profile                      = profiles.get(ui_->profiles->currentText());
        profile.getGeneralSettings().eGame = ui_->games->itemData(idx).value<Games>();
    });

    connect(ui_->profiles, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        selectRightGame(ui_->profiles->itemText(idx));
    });

    connect(ui_->newPushButton, &QPushButton::pressed, this, &ProfilesManagerWindow::createProfile);
    connect(ui_->removePushButton, &QPushButton::pressed, this, &ProfilesManagerWindow::deleteCurrentProfile);
    connect(ui_->renamePushButton, &QPushButton::pressed, this, &ProfilesManagerWindow::renameCurrentProfile);

    updateProfiles(*ui_->profiles);
    selectRightGame(ui_->profiles->currentText());
}

void ProfilesManagerWindow::updateProfiles(QComboBox &box)
{
    box.clear();
    box.addItems(profiles.list());
    selectText(box, profiles.currentProfileName());
}

QString ProfilesManagerWindow::getSelectedProfile()
{
    return ui_->profiles->currentText();
}

void ProfilesManagerWindow::setEnabledProfile(const QString &name)
{
    selectText(*ui_->profiles, name);
}

void ProfilesManagerWindow::setAllowedGames(const std::vector<Games> &games)
{
    auto &gameBox = ui_->games;

    for (int i = 0; i < gameBox->count(); ++i)
        setItemEnabled(*gameBox, i, false);

    for (auto game : games)
    {
        int idx = findData(*gameBox, game);
        setItemEnabled(*gameBox, idx, false);
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

    this->profiles.create(text, baseProfile);
    setProfile(text);
}

void ProfilesManagerWindow::setProfile(const QString &name)
{
    updateProfiles(*ui_->profiles);
    ui_->profiles->setCurrentIndex(ui_->profiles->findText(name));
}

void ProfilesManagerWindow::deleteCurrentProfile()
{
    const QString &current = ui_->profiles->currentText();
    const auto button      = QMessageBox::warning(
        this,
        tr("Remove profile"),
        tr("Are you sure you want to remove profile '%1'? This action cannot be undone").arg(current),
        QMessageBox::No | QMessageBox::Yes);

    if (button != QMessageBox::Yes)
        return;

    if (!profiles.remove(current))
        QMessageBox::critical(this, tr("Removal failed"), tr("Failed to delete profile: %1").arg(current));

    updateProfiles(*ui_->profiles);
}

void ProfilesManagerWindow::renameCurrentProfile()
{
    const QString &text = QInputDialog::getText(this, tr("Rename profile"), tr("New name:"));

    if (text.isEmpty())
        return;

    const QString &current = ui_->profiles->currentText();
    if (!profiles.rename(current, text))
        QMessageBox::critical(this, tr("Renaming failed"), tr("Failed to rename profile: %1").arg(current));

    updateProfiles(*ui_->profiles);
}

void ProfilesManagerWindow::selectRightGame(const QString &profileName)
{
    if (!profiles.exists(profileName))
        return;

    auto &profile = profiles.get(profileName);
    selectData(*ui_->games, profile.getGeneralSettings().eGame());
}

} // namespace CAO
