/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "MainWindowte.h"
#include "ui_MainWindowte.h"

MainWindowte::MainWindowte(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindowte)
{
    ui->setupUi(this);
}

MainWindowte::~MainWindowte()
{
    delete ui;
}
