/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Skyrim.h"
#include "ui_Skyrim.h"

#include <QDebug>

Skyrim::Skyrim() : ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Loading remembered settings TODO change ini path
    settings = new QSettings("settings/SkyrimSE.ini", QSettings::IniFormat, this);

    if(!settings->contains("iLogLevel"))
        settings->setValue("iLogLevel", 3);
    this->loadUIFromFile();

    //Preparing log

    ui->plainTextEdit->setReadOnly(true);
    ui->plainTextEdit->setMaximumBlockCount(25);
    updateLog();

    //Connecting checkboxes to file

    connect(ui->BsaGroupBox, &QGroupBox::clicked, this, &Skyrim::saveUIToFile);
    connect(ui->extractBsaCheckbox, &QCheckBox::clicked, this, &Skyrim::saveUIToFile);
    connect(ui->createtBsaCheckbox, &QCheckBox::clicked, this, &Skyrim::saveUIToFile);
    connect(ui->bsaDeleteBackupsCheckbox, &QCheckBox::clicked, this, &Skyrim::saveUIToFile);

    connect(ui->texturesGroupBox, &QGroupBox::clicked, this, &Skyrim::saveUIToFile);
    connect(ui->TexturesNecessaryOptimizationRadioButton, &QCheckBox::clicked, this, &Skyrim::saveUIToFile);

    connect(ui->meshesGroupBox, &QGroupBox::clicked, this, &Skyrim::saveUIToFile);
    connect(ui->MeshesNecessaryOptimizationRadioButton, &QCheckBox::clicked, this, &Skyrim::saveUIToFile);
    connect(ui->MeshesFullOptimizationRadioButton, &QCheckBox::clicked, this, &Skyrim::saveUIToFile);

    connect(ui->animationsGroupBox, &QGroupBox::clicked, this, &Skyrim::saveUIToFile);
    connect(ui->animationOptimizationRadioButton, &QCheckBox::clicked, this, &Skyrim::saveUIToFile);

    //Connecting the other widgets

    connect(ui->dryRunCheckBox, &QCheckBox::clicked, this, &Skyrim::saveUIToFile);
    connect(ui->userPathTextEdit, &QLineEdit::textChanged, this, &Skyrim::saveUIToFile);

    connect(ui->modeChooserComboBox, QOverload<int>::of(&QComboBox::activated), this, [&]
    {
        if(ui->modeChooserComboBox->currentIndex() == 1)
        {
            QMessageBox warning(this);
            warning.setText(tr("You have selected the several mods option. This process may take a very long time, especially if you process BSA. "
                               "\nThis process has only been tested on the Mod Organizer mods folder."));
            warning.exec();
        }
        this->saveUIToFile();
    });


    connect(ui->userPathButton, &QPushButton::pressed, this, [&](){
        QString dir = QFileDialog::getExistingDirectory(this, "Open Directory",
                                                        settings->value("SelectedPath").toString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if(!dir.isEmpty()) ui->userPathTextEdit->setText(dir);
        this->saveUIToFile();
    });

    connect(ui->processButton, &QPushButton::pressed, this, [&]()
    {
        if(QDir(ui->userPathTextEdit->text()).exists())
            this->initProcess();
        else
            QMessageBox::critical(this, tr("Non existing path"), tr("This path does not exist. Process aborted."), QMessageBox::Ok);

    });

    //Connecting menu buttons

    connect(ui->actionChangeTheme, &QAction::triggered, this, [&]()
    {
        bDarkMode = !bDarkMode;
        this->saveUIToFile();
    });


    connect(ui->actionLogVerbosityInfo, &QAction::triggered, this, [&](){this->settings->setValue("iLogLevel", 4);});
    connect(ui->actionLogVerbosityInfo, &QAction::triggered, this, &Skyrim::loadUIFromFile);

    connect(ui->actionLogVerbosityNote, &QAction::triggered, this, [&](){this->settings->setValue("iLogLevel", 3);});
    connect(ui->actionLogVerbosityNote, &QAction::triggered, this, &Skyrim::loadUIFromFile);

    connect(ui->actionLogVerbosityTrace, &QAction::triggered, this, [&](){this->settings->setValue("iLogLevel", 0);});
    connect(ui->actionLogVerbosityTrace, &QAction::triggered, this, &Skyrim::loadUIFromFile);

    //Setting caoProcess

    caoProcess = new QProcess(this);
    caoProcess->setProgram("bin/Cathedral_Assets_Optimizer_back.exe");

    QObject::connect(caoProcess, &QProcess::readyReadStandardOutput, this, [&]()
    {
        QString readLine = QString::fromLocal8Bit(caoProcess->readLine());
        ui->progressBar->setValue(readLine.toInt());
    });

    QObject::connect(caoProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [&]()
    {
        ui->processButton->setDisabled(false);
        bLockVariables = false;
        updateLog();
    });
}


void Skyrim::initProcess()
{
    caoProcess->start();
    ui->processButton->setDisabled(true);
    bLockVariables = true;
}

void Skyrim::saveUIToFile()
{
    if(bLockVariables)
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
    settings->setValue("iTexturesOptimizationLevel", 2);
    if(!ui->texturesGroupBox->isChecked())
        settings->setValue("iTexturesOptimizationLevel", 0);

    settings->endGroup();

    //Meshes

    settings->beginGroup("Meshes");

    if(ui->meshesGroupBox->isChecked())
        settings->setValue("iMeshesOptimizationLevel", 1);
    if(ui->MeshesFullOptimizationRadioButton->isChecked())
        settings->setValue("iMeshesOptimizationLevel", 3);
    if(!ui->meshesGroupBox->isChecked())
        settings->setValue("iMeshesOptimizationLevel", 0);

    settings->endGroup();

    //Animations

    settings->beginGroup("Animations");
    settings->setValue("bAnimationsOptimization", ui->animationsGroupBox->isChecked());
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

    this->loadUIFromFile();
}

void Skyrim::loadUIFromFile()//Apply the Optimiser settings to the checkboxes
{
    ui->userPathTextEdit->setText(settings->value("SelectedPath").toString());
    settings->setValue("game", "tes5");

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
    case 1:
    case 2:
        ui->texturesGroupBox->setChecked(true);     ui->TexturesNecessaryOptimizationRadioButton->setChecked(true);  break;
    }
    settings->endGroup();

    //Meshes

    settings->beginGroup("Meshes");
    switch(settings->value("iMeshesOptimizationLevel").toInt())
    {
    case 0: ui->meshesGroupBox->setChecked(false); break;
    case 1: ui->meshesGroupBox->setChecked(true);     ui->MeshesNecessaryOptimizationRadioButton->setChecked(true);  break;
    case 3: ui->meshesGroupBox->setChecked(true);     ui->MeshesFullOptimizationRadioButton->setChecked(true); break;
    }
    settings->endGroup();

    //Animations

    settings->beginGroup("Animations");
    ui->animationsGroupBox->setChecked(settings->value("bAnimationsOptimization").toBool());
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

    //Disabling some meshes options when several mods mode is enabled

    if(settings->value("iMode").toInt() == 1)
    {
        ui->MeshesFullOptimizationRadioButton->setDisabled(true);
        ui->MeshesNecessaryOptimizationRadioButton->setChecked(true);
    }
    else if(ui->meshesGroupBox->isChecked())
        ui->MeshesFullOptimizationRadioButton->setDisabled(false);

    //Disabling BSA options if dry run is enabled

    if(ui->dryRunCheckBox->isChecked())
    {
        ui->BsaGroupBox->setChecked(false);
        ui->BsaGroupBox->setEnabled(false);
    }
    else
        ui->BsaGroupBox->setEnabled(true);

    // TODO add headparts checkbox to UI
}



void Skyrim::updateLog()
{
    ui->plainTextEdit->clear();

    QFile log("logs/log.html");
    if(log.open(QFile::Text | QFile::ReadOnly))
    {
        QTextStream ts(&log);
        ts.setCodec(QTextCodec::codecForName("UTF-8"));
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
        ui->plainTextEdit->appendHtml(ts.readAll());
        log.close();
    }
}

Skyrim::~Skyrim()
{
    delete ui;
}



