/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QWidget>

namespace Ui {
class BSAFilesToPack;
}

class BSAFilesToPackWidget : public QWidget
{
public:
    explicit BSAFilesToPackWidget(QWidget *parent = nullptr);
    ~BSAFilesToPackWidget();
    Ui::BSAFilesToPack &getUi() { return *ui; }

private:
    Ui::BSAFilesToPack *ui;
};

