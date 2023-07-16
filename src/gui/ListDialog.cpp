/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "ListDialog.hpp"

#include "ui_ListDialog.h"

#include <QInputDialog>

namespace cao {
ListDialog::ListDialog(bool sortByText, QWidget *parent)
    : QDialog(parent)
    , ui_(new Ui::ListDialog)
    , sortByText_(sortByText)
{
    ui_->setupUi(this);
    connect(ui_->lineEdit, &QLineEdit::textEdited, this, &ListDialog::filterView);
    connect(ui_->addItemButton, &QPushButton::clicked, this, [this] { this->addUserItem(); });
    connect(ui_->closeButton, &QPushButton::clicked, this, &QDialog::accept);

    //Always keeping checked items at the top
    connect(ui_->listWidget, &QListWidget::itemChanged, this, [this](auto *item) {
        auto *extractedItem = ui_->listWidget->takeItem(ui_->listWidget->row(item));
        ui_->listWidget->insertItem(findInsertPos(extractedItem), extractedItem);
    });
}

ListDialog::~ListDialog() = default;

void ListDialog::setUserAddItemVisible(bool visible)
{
    ui_->addItemButton->setVisible(visible);
}

void ListDialog::addItem(QListWidgetItem *item)
{
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);

    //Weird workaround to display the checkbox. It isn't displayed otherwise
    if (item->checkState() != Qt::Checked)
        item->setCheckState(Qt::Unchecked);

    ui_->listWidget->insertItem(findInsertPos(item), item);
}

void ListDialog::addUserItem()
{
    bool ok = false;
    const QString &text = QInputDialog::getText(this, tr("New item"), tr("Name:"), QLineEdit::Normal, "", &ok);
    if (!ok || text.isEmpty())
        return;

    auto item = new QListWidgetItem(text);
    item->setCheckState(Qt::Checked);
    ui_->listWidget->addItem(item);
}

void ListDialog::filterView(const QString &text)
{
    for (int i = 0; i < ui_->listWidget->count(); ++i)
        ui_->listWidget->item(i)->setHidden(true);

    const auto matches(ui_->listWidget->findItems(text, Qt::MatchFlag::MatchContains));
    for (QListWidgetItem *item : matches)
        item->setHidden(false);
}

int ListDialog::findInsertPos(const QListWidgetItem *item)
{
    auto *list = ui_->listWidget;

    for (int i = 0, count = list->count(); i < count; ++i)
    {
        auto *other = list->item(i);
        if (other->checkState() != item->checkState())
            continue;

        if (!sortByText_)
            return i;

        if (item > other)
            return i;
    }

    return 0;
}

std::vector<const QListWidgetItem *> ListDialog::getChoices()
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

std::vector<QListWidgetItem *> ListDialog::items()
{
    std::vector<QListWidgetItem *> result;
    result.reserve(ui_->listWidget->count());

    for (int i = 0; i < ui_->listWidget->count(); ++i)
        result.emplace_back(ui_->listWidget->item(i));

    return result;
}

void ListDialog::setCheckedItems(const QStringList &textList, bool addMissingItems)
{
    for (const QString &string : textList)
        setCheckedItems(string, addMissingItems);
}

void ListDialog::setCheckedItems(const QString &text, bool addMissingItems)
{
    const auto &list = ui_->listWidget->findItems(text, Qt::MatchExactly);

    for (auto &item : list)
        item->setCheckState(Qt::Checked);

    if (list.isEmpty() && addMissingItems)
    {
        auto newItem = new QListWidgetItem();
        newItem->setCheckState(Qt::Checked);
        ui_->listWidget->addItem(text);
    }
}

} // namespace cao
