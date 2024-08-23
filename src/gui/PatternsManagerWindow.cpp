/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "PatternsManagerWindow.hpp"

#include "ui_PatternsManagerWindow.h"
#include "utils/utils.hpp"

#include <QInputDialog>
#include <QMessageBox>

namespace cao {
PatternItem::PatternItem(PerFileSettings &pfs, QListWidget *parent)
    : QListWidgetItem(to_qstring(pfs.pattern.text()), parent, UserType)
    , pfs_(pfs)
{
    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);

    // Distinguish regex patterns somehow.
    if (pfs.pattern.type() == Pattern::Type::Regex)
    {
        auto f = font();
        f.setBold(true);
        setFont(f);
    }
}

const std::u8string PatternItem::update_text()
{
    const auto old = pfs_.pattern.text();
    pfs_.pattern   = Pattern{to_u8string(text()), pfs_.pattern.type()};
    return old;
}

PatternsManagerWindow::PatternsManagerWindow(Settings &settings, QWidget *parent)
    : QDialog(parent)
    , ui_(std::make_unique<Ui::PatternsManagerWindow>())
    , settings_(settings)
{
    ui_->setupUi(this);
    ui_->removePushButton->setEnabled(false);

    connect(ui_->patterns, &QListWidget::currentRowChanged, this, [&](auto current) {
        if (current > -1)
            ui_->removePushButton->setEnabled(true);
        else
            ui_->removePushButton->setEnabled(false);
    });
    connect(ui_->patterns, &QListWidget::itemChanged, this, &PatternsManagerWindow::update_pattern);
    connect(ui_->patterns->model(),
            &QAbstractItemModel::rowsMoved,
            this,
            &PatternsManagerWindow::move_pattern);
    connect(ui_->newPushButton, &QPushButton::pressed, this, &PatternsManagerWindow::create_pattern);
    connect(ui_->removePushButton,
            &QPushButton::pressed,
            this,
            &PatternsManagerWindow::delete_current_pattern);

    update_patterns(*ui_->patterns);
}

PatternsManagerWindow::~PatternsManagerWindow() = default;

void PatternsManagerWindow::update_patterns(QListWidget &list)
{
    // Clear current item first to fix an issue with currentRowChanged signal.
    list.setCurrentItem(nullptr);
    list.clear();
    auto per_file_settings = settings_.current_profile().per_file_settings();
    for (PerFileSettings *pfs : per_file_settings)
    {
        if (pfs->pattern.text() == k_default_pattern.text())
            continue;

        list.addItem(new PatternItem(*pfs));
    }
}

void PatternsManagerWindow::update_pattern(QListWidgetItem *item)
{
    const auto old_pattern = static_cast<PatternItem *>(item)->update_text();
    const auto new_pattern = to_u8string(item->text());

    if (settings_.gui.selected_pattern == old_pattern && old_pattern != new_pattern)
        settings_.gui.selected_pattern = new_pattern;
}

void PatternsManagerWindow::move_pattern(
    const QModelIndex, int source_row, int, const QModelIndex, int destination_row)
{
    settings_.current_profile().move_per_file_settings(source_row, destination_row);
    update_patterns(*ui_->patterns);
}

void PatternsManagerWindow::create_pattern()
{
    bool ok = false;
    const QString &pat_name
        = QInputDialog::getText(this, tr("New Pattern"), tr("Name:"), QLineEdit::Normal, "", &ok);
    if (!ok || pat_name.isEmpty())
        return;

    const auto new_pfs_name = to_u8string(pat_name);

    // Check whether such pattern already exists.
    auto all_pfs = settings_.current_profile().per_file_settings();
    auto found   = std::ranges::find(all_pfs, new_pfs_name, [](const PerFileSettings *pfs) {
        return pfs->pattern.text();
    });

    if (found != all_pfs.end())
        return;

    //Choosing Pattern type

    QStringList type_list{"Glob", "Regex"};
    const QString type_str = QInputDialog::getItem(this,
                                                   tr("Pattern type"),
                                                   tr("What kind of a pattern would you like to create?"),
                                                   type_list,
                                                   0,
                                                   false,
                                                   &ok);
    if (!ok)
        return;

    Pattern::Type pattern_type;
    if (type_str == "Glob")
        pattern_type = Pattern::Type::Wildcard;
    else if (type_str == "Regex")
        pattern_type = Pattern::Type::Regex;
    else
        return;

    //Choosing base Pattern

    QStringList patterns_list;
    for (auto pfs : all_pfs)
        patterns_list.push_back(to_qstring(pfs->pattern.text()));

    const auto &patterns = ui_->patterns;

    const QString base_pfs_qs = QInputDialog::getItem(this,
                                                      tr("Base pattern"),
                                                      tr("Which pattern do you want to use as a base?"),
                                                      patterns_list,
                                                      patterns->currentRow(),
                                                      false,
                                                      &ok);

    if (!ok)
        return;

    const auto base_pfs = to_u8string(base_pfs_qs);

    PerFileSettings new_pfs = **std::ranges::find(all_pfs, base_pfs, [](const PerFileSettings *pfs) {
        return pfs->pattern.text();
    });

    new_pfs.pattern = Pattern{new_pfs_name, pattern_type};

    settings_.current_profile().prepend_per_file_settings(std::move(new_pfs));
    update_patterns(*ui_->patterns);
}

void PatternsManagerWindow::delete_current_pattern()
{
    const QString current = ui_->patterns->currentItem()->text();

    // Ensure user cannot delete default pattern to avoid issues.
    if (to_u8string(current) == k_default_pattern.text())
        return;

    const auto button = QMessageBox::warning(
        this,
        tr("Remove Pattern"),
        tr("Are you sure you want to remove Pattern '%1'? This action cannot be undone").arg(current),
        QMessageBox::No | QMessageBox::Yes);

    if (button != QMessageBox::Yes)
        return;

    // Ensure that the current selected pattern is correct to avoid crashes.
    if (current_per_file_settings(settings_).pattern.text() == to_u8string(current))
        settings_.gui.selected_pattern = k_default_pattern.text();

    settings_.current_profile().remove_per_file_settings(to_u8string(current));
    update_patterns(*ui_->patterns);
}

} // namespace cao
