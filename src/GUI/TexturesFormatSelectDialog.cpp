/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "TexturesFormatSelectDialog.hpp"
#include "GUI/ui_TexturesFormatSelectDialog.h"

namespace CAO {
TexturesFormatSelectDialog::TexturesFormatSelectDialog(QWidget *parent)
    : QDialog(parent)
    , _ui(new ::Ui::TexturesFormatSelectDialog)
{
    _ui->setupUi(this);

    connect(_ui->lineEdit, &QLineEdit::textEdited, this, &TexturesFormatSelectDialog::search);

    for (const auto &value : Detail::DxgiFormats)
    {
        auto item = new QListWidgetItem(value.name);
        item->setData(Qt::UserRole, value.format);
        item->setCheckState(Qt::Unchecked);
        _ui->listWidget->addItem(item);
    }
};

void TexturesFormatSelectDialog::search(const QString &text)
{
    for (int i = 0; i < _ui->listWidget->count(); ++i)
        _ui->listWidget->item(i)->setHidden(true);

    QList<QListWidgetItem *> matches(_ui->listWidget->findItems(text, Qt::MatchFlag::MatchContains));
    for (QListWidgetItem *item : matches)
        item->setHidden(false);
}

QStringList TexturesFormatSelectDialog::getChoices()
{
    QStringList items;
    for (int i = 0; i < _ui->listWidget->count(); ++i)
    {
        auto item = _ui->listWidget->item(i);
        if (item->checkState() == Qt::Checked)
            items << item->text();
    }
    return items;
}

void TexturesFormatSelectDialog::setCheckedItems(const QStringList &textList)
{
    for (const QString &string : textList)
        setCheckedItems(string);
}

void TexturesFormatSelectDialog::setCheckedItems(const QString &text)
{
    for (auto &item : _ui->listWidget->findItems(text, Qt::MatchExactly))
        item->setCheckState(Qt::Checked);
}

TexturesFormatSelectDialog::~TexturesFormatSelectDialog()
{
    delete _ui;
}
} // namespace CAO
