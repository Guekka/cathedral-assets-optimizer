/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "ListDialog.hpp"

#include "ui_ListDialog.h"

#include <QInputDialog>

namespace cao {
ListDialog::ListDialog(Sorting sorting, QWidget *parent)
    : QDialog(parent)
    , ui_(new Ui::ListDialog)
    , sorting_(sorting)
{
    ui_->setupUi(this);
    connect(ui_->lineEdit, &QLineEdit::textEdited, this, &ListDialog::filter_view);
    connect(ui_->addItemButton, &QPushButton::clicked, this, &ListDialog::add_user_item);
    connect(ui_->closeButton, &QPushButton::clicked, this, &QDialog::accept);

    //Always keeping checked items at the top
    connect(ui_->listWidget, &QListWidget::itemChanged, this, [this](auto *item) {
        auto *extracted_item = ui_->listWidget->takeItem(ui_->listWidget->row(item));
        ui_->listWidget->insertItem(find_insert_pos(extracted_item), extracted_item);
    });
}

ListDialog::~ListDialog() = default;

void ListDialog::set_user_add_item_visible(bool visible)
{
    ui_->addItemButton->setVisible(visible);
}

void ListDialog::add_item(QListWidgetItem *item)
{
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);

    //Weird workaround to display the checkbox. It isn't displayed otherwise
    if (item->checkState() != Qt::Checked)
        item->setCheckState(Qt::Unchecked);

    ui_->listWidget->insertItem(find_insert_pos(item), item);
}

void ListDialog::add_user_item()
{
    bool ok = false;
    const QString &text = QInputDialog::getText(this, tr("New item"), tr("Name:"), QLineEdit::Normal, "", &ok);
    if (!ok || text.isEmpty())
        return;

    auto *item = new QListWidgetItem(text, ui_->listWidget); //NOLINT(cppcoreguidelines-owning-memory)
    item->setCheckState(Qt::Checked);
    ui_->listWidget->addItem(item);
}

void ListDialog::filter_view(const QString &text)
{
    for (int i = 0; i < ui_->listWidget->count(); ++i)
        ui_->listWidget->item(i)->setHidden(true);

    const auto matches(ui_->listWidget->findItems(text, Qt::MatchFlag::MatchContains));
    for (QListWidgetItem *item : matches)
        item->setHidden(false);
}

auto ListDialog::find_insert_pos(const QListWidgetItem *item) -> int
{
    auto *list = ui_->listWidget;

    for (int i = 0, count = list->count(); i < count; ++i)
    {
        auto *other = list->item(i);
        if (other->checkState() != item->checkState())
            continue;

        if (sorting_ == Sorting::Insertion)
            return i;

        if (item > other)
            return i;
    }

    return 0;
}

auto ListDialog::get_choices() -> std::vector<const QListWidgetItem *>
{
    std::vector<const QListWidgetItem *> items;

    for (int i = 0; i < ui_->listWidget->count(); ++i)
    {
        auto *item = ui_->listWidget->item(i);
        if (item->checkState() == Qt::Checked)
            items.emplace_back(item);
    }
    return items;
}

auto ListDialog::items() -> std::vector<QListWidgetItem *>
{
    std::vector<QListWidgetItem *> result;
    result.reserve(ui_->listWidget->count());

    for (int i = 0; i < ui_->listWidget->count(); ++i)
        result.emplace_back(ui_->listWidget->item(i));

    return result;
}

void ListDialog::set_checked_items(const QStringList &text_list, bool add_missing_items)
{
    for (const QString &string : text_list)
        set_checked_items(string, add_missing_items);
}

void ListDialog::set_checked_items(const QString &text, bool add_missing_items)
{
    const auto &list = ui_->listWidget->findItems(text, Qt::MatchExactly);

    for (const auto &item : list)
        item->setCheckState(Qt::Checked);

    if (list.isEmpty() && add_missing_items)
    {
        auto *new_item = new QListWidgetItem(text, ui_->listWidget); //NOLINT(cppcoreguidelines-owning-memory)
        new_item->setCheckState(Qt::Checked);
        ui_->listWidget->addItem(new_item);
    }
}

} // namespace cao
