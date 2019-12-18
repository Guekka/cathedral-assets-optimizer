/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "BSAFilesToPack.hpp"
#include "ui_BSAFilesToPack.h"

BSAFilesToPackWidget::BSAFilesToPackWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BSAFilesToPack)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, [this] { this->hide(); });
}

BSAFilesToPackWidget::~BSAFilesToPackWidget()
{
    delete ui;
}
