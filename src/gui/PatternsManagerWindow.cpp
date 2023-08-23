/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "PatternsManagerWindow.hpp"

#include "settings/profile.hpp"
#include "settings/settings.hpp"
#include "ui_PatternsManagerWindow.h"
#include "utils/utils.hpp"

#include <QInputDialog>
#include <QMessageBox>

namespace cao {
PatternsManagerWindow::PatternsManagerWindow(Settings &settings, QWidget *parent)
    : QDialog(parent)
    , ui_(std::make_unique<Ui::PatternsManagerWindow>())
    , settings_(settings)
{
    ui_->setupUi(this);

    connect(ui_->newPushButton, &QPushButton::pressed, this, &PatternsManagerWindow::create_pattern);
    connect(ui_->removePushButton,
            &QPushButton::pressed,
            this,
            &PatternsManagerWindow::delete_current_pattern);

    update_patterns(*ui_->patterns);

    const bool success = select_text(*ui_->patterns,
                                     to_qstring(current_per_file_settings(settings_).pattern.text()));
    assert(success);
}

PatternsManagerWindow::~PatternsManagerWindow() = default;

void PatternsManagerWindow::update_patterns(QComboBox &box)
{
    box.clear();

    box.addItem(to_qstring(k_default_pattern.text()));

    for (const auto *pfs : settings_.current_profile().per_file_settings())
        box.addItem(to_qstring(pfs->pattern.text()));

    const bool success = select_text(box, to_qstring(current_per_file_settings(settings_).pattern.text()));
    assert(success);
}

void PatternsManagerWindow::create_pattern()
{
    bool ok = false;
    const QString &pat_name
        = QInputDialog::getText(this, tr("New Pattern"), tr("Name:"), QLineEdit::Normal, "", &ok);
    if (!ok || pat_name.isEmpty())
        return;

    //Choosing base Pattern

    QStringList patterns_list;
    auto &patterns = ui_->patterns;
    for (int i = 0; i < patterns->count(); ++i)
        patterns_list.push_back(patterns->itemText(i));

    const QString base_pfs_qs = QInputDialog::getItem(this,
                                                      tr("Base pattern"),
                                                      tr("Which pattern do you want to use as a base?"),
                                                      patterns_list,
                                                      patterns->currentIndex(),
                                                      false,
                                                      &ok);

    if (!ok)
        return;

    const auto base_pfs = to_u8string(base_pfs_qs);

    auto all_pfs            = settings_.current_profile().per_file_settings();
    PerFileSettings new_pfs = **std::ranges::find(all_pfs, base_pfs, [](const PerFileSettings *pfs) {
        return pfs->pattern.text();
    });

    new_pfs.pattern = Pattern{base_pfs};

    settings_.current_profile().add_per_file_settings(std::move(new_pfs));
    update_patterns(*ui_->patterns);
}

void PatternsManagerWindow::delete_current_pattern()
{
    const QString current = ui_->patterns->currentText();
    const auto button     = QMessageBox::warning(
        this,
        tr("Remove Pattern"),
        tr("Are you sure you want to remove Pattern '%1'? This action cannot be undone").arg(current),
        QMessageBox::No | QMessageBox::Yes);

    if (button != QMessageBox::Yes)
        return;

    settings_.current_profile().remove_per_file_settings(to_u8string(current));
    update_patterns(*ui_->patterns);
}

} // namespace cao
