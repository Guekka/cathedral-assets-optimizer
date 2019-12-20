/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "ListDialog.hpp"
#include "ui_ListDialog.h"

namespace CAO {
ListDialog::ListDialog(QWidget *parent)
    : QDialog(parent)
    , _ui(new Ui::ListDialog)
{
    _ui->setupUi(this);
    connect(_ui->lineEdit, &QLineEdit::textEdited, this, &ListDialog::search);
    connect(_ui->addItemButton, &QPushButton::clicked, this, [this] { this->addItem(); });
    connect(_ui->closeButton, &QPushButton::clicked, this, [this] {
        emit accepted();
        close();
    });
}

ListDialog::~ListDialog()
{
    delete _ui;
}

void ListDialog::setUserAddItemAllowed(bool allowed)
{
    _ui->addItemButton->setEnabled(allowed);
}

void ListDialog::addItem(QListWidgetItem *item)
{
    item->setCheckState(Qt::Unchecked);
    _ui->listWidget->addItem(item);
    _ui->listWidget->sortItems();
}

void ListDialog::addItem()
{
    bool ok = false;
    const QString &text = QInputDialog::getText(this, tr("New item"), tr("Name:"), QLineEdit::Normal, "", &ok);
    if (!ok || text.isEmpty())
        return;

    auto item = new QListWidgetItem(text);
    item->setCheckState(Qt::Checked);
    _ui->listWidget->addItem(item);
}

void ListDialog::search(const QString &text)
{
    for (int i = 0; i < _ui->listWidget->count(); ++i)
        _ui->listWidget->item(i)->setHidden(true);

    QList<QListWidgetItem *> matches(_ui->listWidget->findItems(text, Qt::MatchFlag::MatchContains));
    for (QListWidgetItem *item : matches)
        item->setHidden(false);
}

QVector<QListWidgetItem *> ListDialog::getChoices()
{
    QVector<QListWidgetItem *> items;
    for (int i = 0; i < _ui->listWidget->count(); ++i)
    {
        auto item = new QListWidgetItem(*_ui->listWidget->item(i));
        if (item->checkState() == Qt::Checked)
            items << item;
    }
    return items;
}

void ListDialog::setCheckedItems(const QStringList &textList)
{
    for (const QString &string : textList)
        setCheckedItems(string);
}

void ListDialog::setCheckedItems(const QString &text)
{
    const auto &list = _ui->listWidget->findItems(text, Qt::MatchExactly);
    for (auto &item : list)
        item->setCheckState(Qt::Checked);
    if (list.isEmpty())
    {
        auto newItem = new QListWidgetItem(text);
        newItem->setCheckState(Qt::Checked);
    }
}

} // namespace CAO
