/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"
#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
    class TexturesFormatSelectDialog;
}

class TexturesFormatSelectDialog : public QDialog
{   
public:
    explicit TexturesFormatSelectDialog(QWidget *parent = nullptr);
    ~TexturesFormatSelectDialog();

    void search(const QString &text);
    QStringList getChoices();
    void setCheckedItems(const QString &text);
    void setCheckedItems(const QStringList &textList);

private:
    Ui::TexturesFormatSelectDialog *_ui;
};

