/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "PatternsManagerWindow.hpp"
#include "Settings/Profile.hpp"
#include "Utils.hpp"

namespace CAO {
PatternsManagerWindow::PatternsManagerWindow(Profile &profile_, QWidget *parent)
    : QDialog(parent)
    , ui_(new Ui::PatternsManagerWindow)
    , profile(profile_)
{
    ui_->setupUi(this);

    connect(ui_->newPushButton, &QPushButton::pressed, this, &PatternsManagerWindow::createPattern);
    connect(ui_->removePushButton, &QPushButton::pressed, this, &PatternsManagerWindow::deleteCurrentPattern);

    updatePatterns(*ui_->patterns);
}

void PatternsManagerWindow::updatePatterns(QComboBox &box)
{
    box.clear();
    box.addItems(profile.getPatterns().list());
    selectText(box, profile.getGeneralSettings().sCurrentPattern());
}

void PatternsManagerWindow::createPattern()
{
    bool ok = false;
    const QString &patName
        = QInputDialog::getText(this, tr("New Pattern"), tr("Name:"), QLineEdit::Normal, "", &ok);
    if (!ok || patName.isEmpty())
        return;

    //Choosing base Pattern

    QStringList PatternsList;
    auto &patterns = ui_->patterns;
    for (int i = 0; i < patterns->count(); ++i)
        PatternsList.push_back(patterns->itemText(i));

    const QString &basePattern = QInputDialog::getItem(this,
                                                       tr("Base pattern"),
                                                       tr("Which pattern do you want to use as a base?"),
                                                       PatternsList,
                                                       patterns->currentIndex(),
                                                       false,
                                                       &ok);

    if (!ok)
        return;

    PatternSettings newSets = profile.getPatterns().getSettingsByName(basePattern);
    newSets.pattern         = patName.toStdString();
    profile.getPatterns().addPattern(newSets);

    updatePatterns(*ui_->patterns);
}

void PatternsManagerWindow::deleteCurrentPattern()
{
    const QString &current = ui_->patterns->currentText();
    const auto button      = QMessageBox::warning(
        this,
        tr("Remove Pattern"),
        tr("Are you sure you want to remove Pattern '%1'? This action cannot be undone").arg(current),
        QMessageBox::No | QMessageBox::Yes);

    if (button != QMessageBox::Yes)
        return;

    profile.getPatterns().remove(current);

    updatePatterns(*ui_->patterns);
}

void PatternsManagerWindow::setPattern(const QString &name)
{
    updatePatterns(*ui_->patterns);
    ui_->patterns->setCurrentIndex(ui_->patterns->findText(name));
}

} // namespace CAO
