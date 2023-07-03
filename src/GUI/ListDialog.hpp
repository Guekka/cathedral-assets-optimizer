
/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QDialog>
#include <vector>

namespace Ui {
class ListDialog;
}

class QListWidgetItem;

namespace CAO {
class ListDialog : public QDialog
{
public:
    static constexpr bool doSortByText   = true;
    static constexpr bool dontSortByText = false;

    explicit ListDialog(bool sortByText, QWidget *parent = nullptr);
    ~ListDialog();

    void addItem(QListWidgetItem *item);

    std::vector<const QListWidgetItem *> getChoices();
    std::vector<QListWidgetItem *> items();

    void setUserAddItemVisible(bool visible);

    void setCheckedItems(const QString &text, bool addMissingItems = true);
    void setCheckedItems(const QStringList &textList, bool addMissingItems = true);

private:
    void addUserItem();
    void filterView(const QString &text);
    int findInsertPos(const QListWidgetItem *item);

    std::unique_ptr<Ui::ListDialog> ui_;
    bool sortByText_ = false;
};
} // namespace CAO
