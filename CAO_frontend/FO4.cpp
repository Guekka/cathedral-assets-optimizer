/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "FO4.h"

FO4::FO4() : UiBase ("logs/FO4_log.html", "settings/FO4/config.ini"),
    ui(new Ui::FO4)
{
    ui->setupUi(this);
/*
    setupSettings();
    loadUIFromFile();

    //Preparing log

    updateLog();
*/
    //Connecting checkboxes to file

    connect(ui->BsaGroupBox, &QGroupBox::clicked, this, &FO4::saveUIToFile);
    connect(ui->extractBsaCheckbox, &QCheckBox::clicked, this, &FO4::saveUIToFile);
    connect(ui->createtBsaCheckbox, &QCheckBox::clicked, this, &FO4::saveUIToFile);
    connect(ui->bsaDeleteBackupsCheckbox, &QCheckBox::clicked, this, &FO4::saveUIToFile);

    connect(ui->texturesGroupBox, &QGroupBox::clicked, this, &FO4::saveUIToFile);
    connect(ui->TexturesNecessaryOptimizationRadioButton, &QCheckBox::clicked, this, &FO4::saveUIToFile);
    connect(ui->TexturesFullOptimizationRadioButton, &QCheckBox::clicked, this, &FO4::saveUIToFile);

}

void FO4::saveUIToFile()
{
 /*   if(bLockVariables)
        return;

    //BSA

    settings->beginGroup("BSA");

    settings->setValue("BsaGroupBox", ui->BsaGroupBox->isChecked());

    //Disabling BSA options if dry run is enabled
    if(ui->BsaGroupBox->isChecked() && !ui->dryRunCheckBox->isChecked())
    {
        settings->setValue("bBsaExtract", ui->extractBsaCheckbox->isChecked());
        settings->setValue("bBsaCreate", ui->createtBsaCheckbox->isChecked());
        settings->setValue("bBsaDeleteBackup", ui->bsaDeleteBackupsCheckbox->isChecked());
    }
    else
    {
        settings->setValue("bBsaExtract", false);
        settings->setValue("bBsaCreate", false);
        settings->setValue("bBsaDeleteBackup", false);
    }

    settings->endGroup();

    //Textures

    settings->beginGroup("Textures");

    if(ui->texturesGroupBox->isChecked())
        settings->setValue("iTexturesOptimizationLevel", 1);
    if(ui->TexturesFullOptimizationRadioButton->isChecked())
        settings->setValue("iTexturesOptimizationLevel", 2);
    if(!ui->texturesGroupBox->isChecked())
        settings->setValue("iTexturesOptimizationLevel", 0);

    settings->endGroup();

    //Dry run and mode

    settings->setValue("bDryRun", ui->dryRunCheckBox->isChecked());
    settings->setValue("iMode", ui->modeChooserComboBox->currentIndex());
    settings->setValue("SelectedPath", QDir::cleanPath(ui->userPathTextEdit->text()));

    //Disabling some meshes options when several mods mode is enabled

    if(settings->value("General/iMode").toInt() == 1)
    {
        settings->setValue("Meshes/iMeshesOptimizationLevel", 1);
        settings->setValue("Meshes/bMeshesHeadparts", false);
    }
    else
        settings->setValue("Meshes/bMeshesHeadparts", true);

    //GUI

    settings->setValue("bDarkMode", bDarkMode);

    this->loadUIFromFile();*/
}

void FO4::loadUIFromFile()//Apply the Optimiser settings to the checkboxes
{/*
    ui->userPathTextEdit->setText(settings->value("SelectedPath").toString());

    //BSA

    settings->beginGroup("BSA");
    ui->BsaGroupBox->setChecked(settings->value("BsaGroupBox").toBool());
    ui->extractBsaCheckbox->setChecked(settings->value("bBsaExtract").toBool());
    ui->createtBsaCheckbox->setChecked(settings->value("bBsaCreate").toBool());
    ui->bsaDeleteBackupsCheckbox->setChecked(settings->value("bBsaDeleteBackup").toBool());
    settings->endGroup();

    //Textures

    settings->beginGroup("Textures");
    switch (settings->value("iTexturesOptimizationLevel").toInt())
    {
    case 0: ui->texturesGroupBox->setChecked(false); break;
    case 1: ui->texturesGroupBox->setChecked(true);     ui->TexturesNecessaryOptimizationRadioButton->setChecked(true);  break;
    case 2: ui->texturesGroupBox->setChecked(true);     ui->TexturesFullOptimizationRadioButton->setChecked(true); break;
    }
    settings->endGroup();

    //General and GUI

    ui->modeChooserComboBox->setCurrentIndex(settings->value("iMode").toInt());
    ui->dryRunCheckBox->setChecked(settings->value("bDryRun").toBool());
    bDarkMode = settings->value("bDarkMode").toBool();

    //Log level

    ui->actionLogVerbosityInfo->setChecked(false);
    ui->actionLogVerbosityNote->setChecked(false);
    ui->actionLogVerbosityTrace->setChecked(false);

    switch (settings->value("iLogLevel").toInt())
    {
    case 0: ui->actionLogVerbosityTrace->setChecked(true); break;
    case 2: ui->actionLogVerbosityNote->setChecked(true); break;
    case 3: ui->actionLogVerbosityInfo->setChecked(true); break;
    }

    //Enabling dark mode

    if(bDarkMode)
    {
        QFile f(":qdarkstyle/style.qss");
        f.open(QFile::ReadOnly | QFile::Text);
        qApp->setStyleSheet(f.readAll());
        f.close();
        ui->actionChangeTheme->setText(tr("Switch to light theme"));
    }
    else if(!bDarkMode)
    {
        qApp->setStyleSheet("");
        ui->actionChangeTheme->setText(tr("Switch to dark theme"));
    }

    //Disabling BSA options if dry run is enabled

    if(ui->dryRunCheckBox->isChecked())
    {
        ui->BsaGroupBox->setChecked(false);
        ui->BsaGroupBox->setEnabled(false);
    }
    else
        ui->BsaGroupBox->setEnabled(true);*/
}

FO4::~FO4()
{
    delete ui;
}



