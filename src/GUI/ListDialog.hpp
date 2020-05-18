/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"

namespace Ui {
class ListDialog;
}

namespace CAO {
class ListDialog : public QDialog
{
public:
    explicit ListDialog(QWidget *parent = nullptr);
    ~ListDialog();

    void addItem(QListWidgetItem *item);
    void setUserAddItemAllowed(bool allowed);
    void search(const QString &text);
    QVector<QListWidgetItem *> getChoices();
    void setCheckedItems(const QString &text);
    void setCheckedItems(const QStringList &textList);

protected:
    void addItem();
    Ui::ListDialog *_ui;
};
} // namespace CAO
