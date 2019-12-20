/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "BSAFilesToPack.hpp"
#include "ui_BSAFilesToPack.h"

namespace CAO {
BSAFilesToPackWidget::BSAFilesToPackWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BSAFilesToPack)
{
    ui->setupUi(this);
    connect(ui->closeButton, &QPushButton::clicked, this, [this] { this->hide(); });

    auto connectDialogList = [this](QListWidget &list, ListDialog &dialog, QPushButton &button) {
        connect(&button, &QPushButton::clicked, this, [&list, &dialog] {
            for (int i = 0; i < list.count(); ++i)
                dialog.setCheckedItems(list.item(i)->text());
            dialog.exec();
        });

        connect(&dialog, &ListDialog::accepted, this, [&dialog, &list] {
            list.clear();
            for (const auto &item : dialog.getChoices())
            {
                item->setFlags(item->flags() & (~Qt::ItemIsUserCheckable) & (~Qt::Checked));
                item->setData(Qt::CheckStateRole, QVariant()); //Hiding the checkbox
                list.addItem(item);
            }
        });
    };

    standardList = new ListDialog;
    for (const QString &stdExt : allAssets)
        standardList->addItem(new QListWidgetItem(stdExt));
    connectDialogList(*ui->StandardFilesListWidget, *standardList, *ui->StandardFilesEditButton);

    textureList = new ListDialog;
    for (const QString &stdExt : allAssets)
        textureList->addItem(new QListWidgetItem(stdExt));
    connectDialogList(*ui->TexturesFilesListWidget, *textureList, *ui->TexturesFilesEditButton);

    uncompressableList = new ListDialog;
    for (const QString &stdExt : allAssets)
        uncompressableList->addItem(new QListWidgetItem(stdExt));
    connectDialogList(*ui->UncompressableFilesListWidget, *uncompressableList, *ui->UncompressableFilesEditButton);
}

BSAFilesToPackWidget::~BSAFilesToPackWidget()
{
    delete ui;
}
} // namespace CAO
