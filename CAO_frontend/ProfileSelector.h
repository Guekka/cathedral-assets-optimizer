/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QDialog>

namespace Ui {
class ProfileSelector;
}

class ProfileSelector : public QDialog
{
    Q_OBJECT

public:
    explicit ProfileSelector(QWidget *parent = nullptr);
    ~ProfileSelector();

private:
    Ui::ProfileSelector *ui;
};

