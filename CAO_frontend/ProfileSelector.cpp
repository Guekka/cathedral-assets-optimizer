/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "ProfileSelector.h"
#include "ui_ProfileSelector.h"

ProfileSelector::ProfileSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProfileSelector)
{
    ui->setupUi(this);
}

ProfileSelector::~ProfileSelector()
{
    delete ui;
}
