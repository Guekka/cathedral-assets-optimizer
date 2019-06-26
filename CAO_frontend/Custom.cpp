/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Custom.h"

Custom::Custom() : ui(new Ui::Custom)
{
    ui->setupUi(this);

    //Loading remembered settings
    hideAdvancedSettings();

    commonSettings = new QSettings("settings/common/config.ini", QSettings::IniFormat, this);

    getGameFromIni();
    setGameMode();

    settings = new QSettings(iniPath, QSettings::IniFormat, this);

    if(!settings->contains("iLogLevel"))
        settings->setValue("iLogLevel", 4);

    loadUIFromFile();

    //Connecting widgets

    connect(ui->dryRunCheckBox, &QCheckBox::clicked, this, [&]
    {
        //Disabling BSA options if dry run is enabled

        if(ui->dryRunCheckBox->isChecked())
        {
            ui->bsaBaseGroupBox->setChecked(false);
            ui->bsaBaseGroupBox->setEnabled(false);
        }
        else
            ui->bsaBaseGroupBox->setEnabled(true);

        this->refreshUI();
        this->loadUIFromFile();
    });

    connect(ui->userPathTextEdit, &QLineEdit::textChanged, this, &Custom::refreshUI);

    connect(ui->advancedSettingsCheckbox, &QCheckBox::clicked, this, [&]()
    {
        if(ui->advancedSettingsCheckbox->isChecked())
            this->showAdvancedSettings();
        else
            this->hideAdvancedSettings();
    });

    connect(ui->presets, QOverload<int>::of(&QComboBox::activated), this, [&]
    {
        game = ui->presets->currentText();
        this->setGameMode();
    });

    connect(ui->modeChooserComboBox, QOverload<int>::of(&QComboBox::activated), this, [&]
    {
        if(ui->modeChooserComboBox->currentIndex() == 1)
        {
            QMessageBox warning(this);
            warning.setText(tr("You have selected the several mods option. This process may take a very long time, especially if you process BSA. "
                               "\nThis process has only been tested on the Mod Organizer mods folder."));
            warning.exec();
        }

        this->refreshUI();
    });

    connect(ui->userPathButton, &QPushButton::pressed, this, [&](){
        QString dir = QFileDialog::getExistingDirectory(this, "Open Directory",
                                                        settings->value("SelectedPath").toString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if(!dir.isEmpty()) ui->userPathTextEdit->setText(dir);
        this->refreshUI();
    });

    connect(ui->processButton, &QPushButton::pressed, this, [&]()
    {
        if(QDir(ui->userPathTextEdit->text()).exists())
            this->initProcess();
        else
            QMessageBox::critical(this, tr("Non existing path"), tr("This path does not exist. Process aborted."), QMessageBox::Ok);
    });

    //Connecting menu buttons
    connect(ui->actionEnableDarkTheme, &QAction::triggered, this, &Custom::refreshUI);

    connect(ui->actionLogVerbosityInfo, &QAction::triggered, this, [&](){this->settings->setValue("iLogLevel", 3);});
    connect(ui->actionLogVerbosityInfo, &QAction::triggered, this, &Custom::loadUIFromFile);

    connect(ui->actionLogVerbosityNote, &QAction::triggered, this, [&](){this->settings->setValue("iLogLevel", 4);});
    connect(ui->actionLogVerbosityNote, &QAction::triggered, this, &Custom::loadUIFromFile);

    connect(ui->actionLogVerbosityTrace, &QAction::triggered, this, [&](){this->settings->setValue("iLogLevel", 6);});
    connect(ui->actionLogVerbosityTrace, &QAction::triggered, this, &Custom::loadUIFromFile);

    //Setting caoProcess

    caoProcess = new QProcess(this);
    caoProcess->setProgram("bin/Cathedral_Assets_Optimizer_back.exe");

    //Setting timer to refresh UI
    timer = new QTimer(this);
    timer->start(10000);
    connect(timer, &QTimer::timeout, this, &Custom::readProgress);
    connect(timer, &QTimer::timeout, this, &Custom::updateLog);
    connect(timer, &QTimer::timeout, this, &Custom::refreshUI);
}

void Custom::refreshUI()
{
    //Disabling some meshes options when several mods mode is enabled
    const bool severalModsEnabled = (ui->modeChooserComboBox->currentIndex() == 1);
    ui->meshesMediumOptimizationRadioButton->setDisabled(severalModsEnabled);
    ui->meshesFullOptimizationRadioButton->setDisabled(severalModsEnabled);
    ui->meshesNecessaryOptimizationRadioButton->setChecked(severalModsEnabled);

    //Disabling BSA options if dry run is enabled
    const bool dryRunEnabled = ui->dryRunCheckBox->isChecked();
    ui->bsaExtractCheckBox->setDisabled(dryRunEnabled);
    ui->bsaCreateCheckbox->setDisabled(dryRunEnabled);
    ui->bsaDeleteBackupsCheckbox->setDisabled(dryRunEnabled);

    //Enabling dark mode
    if(ui->actionEnableDarkTheme->isChecked())
    {
        QFile f(":qdarkstyle/style.qss");
        f.open(QFile::ReadOnly | QFile::Text);
        qApp->setStyleSheet(f.readAll());
        f.close();
    }
    else
        qApp->setStyleSheet("");

    if(!bLockVariables)
        saveUIToFile();

    loadUIFromFile();
}

void Custom::saveUIToFile()
{
    //BSA
    settings->beginGroup("BSA");

    settings->setValue("bBsaExtract", ui->bsaExtractCheckBox->isChecked());
    settings->setValue("bBsaCreate", ui->bsaCreateCheckbox->isChecked());
    settings->setValue("bBsaDeleteBackup", ui->bsaDeleteBackupsCheckbox->isChecked());
    settings->setValue("bBsaProcessContent", ui->bsaProcessContentCheckBox->isChecked());

    settings->endGroup();

    //Textures
    int texturesLevel;

    if(ui->texturesNecessaryOptimizationRadioButton->isChecked())
        texturesLevel = 1;
    else if(ui->texturesFullOptimizationRadioButton->isChecked())
        texturesLevel = 2;
    if(!ui->texturesGroupBox->isChecked())
        texturesLevel = 0;

    settings->setValue("Textures/iTexturesOptimizationLevel", texturesLevel);

    //Meshes base
    int meshesLevel;

    if(ui->meshesNecessaryOptimizationRadioButton->isChecked())
        meshesLevel = 1;
    else if(ui->meshesMediumOptimizationRadioButton->isChecked())
        meshesLevel = 2;
    else if(ui->meshesFullOptimizationRadioButton->isChecked())
        meshesLevel = 3;
    if(!ui->meshesGroupBox->isChecked())
        meshesLevel = 0;

    settings->setValue("Meshes/iMeshesOptimizationLevel", meshesLevel);

    //Meshes advanced
    settings->setValue("Meshes/bMeshesHeadparts", ui->meshesHeadpartsCheckBox->isChecked());

    //Animations
    settings->setValue("Animations/bAnimationsOptimization", ui->animationsNecessaryOptimizationCheckBox->isChecked());

    //General
    settings->setValue("SelectedPath", QDir::cleanPath(ui->userPathTextEdit->text()));
    settings->setValue("iMode", ui->modeChooserComboBox->currentIndex());
    settings->setValue("bDryRun", ui->dryRunCheckBox->isChecked());

    //UI
    commonSettings->setValue("bShowAdvancedSettings", ui->advancedSettingsCheckbox->isChecked());
    commonSettings->setValue("bDarkMode", ui->actionEnableDarkTheme->isChecked());
}

void Custom::loadUIFromFile()
{
    ui->userPathTextEdit->setText(settings->value("SelectedPath").toString());

    //BSA
    settings->beginGroup("BSA");
    ui->bsaBaseGroupBox->setChecked(settings->value("bsaBaseGroupBox").toBool());
    ui->bsaExtractCheckBox->setChecked(settings->value("bBsaExtract").toBool());
    ui->bsaCreateCheckbox->setChecked(settings->value("bBsaCreate").toBool());
    ui->bsaDeleteBackupsCheckbox->setChecked(settings->value("bBsaDeleteBackup").toBool());
    settings->endGroup();

    //Textures

    switch (settings->value("Textures/iTexturesOptimizationLevel").toInt())
    {
    case 0: ui->texturesGroupBox->setChecked(false); break;
    case 1: ui->texturesGroupBox->setChecked(true);     ui->texturesNecessaryOptimizationRadioButton->setChecked(true);  break;
    case 2: ui->texturesGroupBox->setChecked(true);     ui->texturesFullOptimizationRadioButton->setChecked(true); break;
    }

    //Meshes

    switch(settings->value("Meshes/iMeshesOptimizationLevel").toInt())
    {
    case 0: ui->meshesGroupBox->setChecked(false); break;
    case 1: ui->meshesGroupBox->setChecked(true);     ui->meshesNecessaryOptimizationRadioButton->setChecked(true);  break;
    case 2: ui->meshesGroupBox->setChecked(true);     ui->meshesMediumOptimizationRadioButton->setChecked(true); break;
    case 3: ui->meshesGroupBox->setChecked(true);     ui->meshesFullOptimizationRadioButton->setChecked(true); break;
    }

    //Animations
    ui->animationsGroupBox->setChecked(settings->value("Animations/bAnimationsOptimization").toBool());

    //General and GUI
    ui->modeChooserComboBox->setCurrentIndex(settings->value("iMode").toInt());
    ui->dryRunCheckBox->setChecked(settings->value("bDryRun").toBool());
    ui->actionEnableDarkTheme->setChecked(commonSettings->value("bDarkMode").toBool());
    ui->advancedSettingsCheckbox->setChecked(commonSettings->value("bShowAdvancedSettings").toBool());

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
}

void Custom::readProgress()
{
    QString readLine;
    while(caoProcess->canReadLine())
        readLine = QString::fromLocal8Bit(caoProcess->readLine());
    QStringList line = readLine.simplified().split('|');
    if(readLine.startsWith("PROGRESS:"))
    {
        ui->progressBar->setFormat(line.at(1));
        int completed = line.at(2).toInt();
        int total = line.at(3).toInt();
        ui->progressBar->setMaximum(total);
        ui->progressBar->setValue(completed);
    }
}

void Custom::initProcess()
{
    caoProcess->start();
    ui->processButton->setDisabled(true);
    bLockVariables = true;
}

void Custom::endProcess()
{
    ui->processButton->setDisabled(false);
    bLockVariables = false;
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(100);
    updateLog();
}

void Custom::closeEvent(QCloseEvent* event)
{
    caoProcess->kill();
    event->accept();
}

void Custom::updateLog()
{
    ui->logTextEdit->clear();

    QFile log(logPath);
    if(log.open(QFile::Text | QFile::ReadOnly))
    {
        QTextStream ts(&log);
        ts.setCodec(QTextCodec::codecForName("UTF-8"));
        ui->logTextEdit->appendHtml(ts.readAll());
        log.close();
    }
}

void Custom::getGameFromIni()
{
    QString iniGame = commonSettings->value("game").toString();
    if(!iniGame.isNull() && !iniGame.isEmpty())
        game = iniGame;
    else
        getGameFromUi();
}

void Custom::getGameFromUi()
{
    game = ui->presets->currentText();
    commonSettings->setValue("game", game);
}

void Custom::setGameMode()
{
    //Resetting the window

    const int animTabIndex =  ui->tabWidget->indexOf(ui->AnimationsTab);
    const int meshesTabIndex = ui->tabWidget->indexOf(ui->meshesTab);
    ui->tabWidget->setTabEnabled(animTabIndex, true);
    ui->tabWidget->setTabEnabled(meshesTabIndex, true);

    ui->texturesFullOptimizationRadioButton->show();
    ui->meshesMediumOptimizationRadioButton->show();

    //Actually setting the window mode

    if(game == "SSE")
    {
        logPath = "logs/SkyrimSE_log.html";
        iniPath = "settings/SkyrimSE/config.ini";
    }
    else if(game == "TES5")
    {
        QTextStream(stdout) << game;

        logPath = "logs/TES5_log.html";
        iniPath = "settings/TES5/config.ini";
        ui->texturesFullOptimizationRadioButton->setChecked(false);
        ui->texturesFullOptimizationRadioButton->hide();

        ui->meshesMediumOptimizationRadioButton->setChecked(false);
        ui->meshesMediumOptimizationRadioButton->hide();

        ui->animationsNecessaryOptimizationCheckBox->setChecked(false);
        ui->tabWidget->setTabEnabled(animTabIndex, false);
    }
    else if(game == "FO4")
    {
        logPath = "logs/FO4_log.html";
        iniPath = "settings/FO4/config.ini";

        ui->animationsNecessaryOptimizationCheckBox->setChecked(false);
        ui->tabWidget->setTabEnabled(animTabIndex, false);

        ui->meshesGroupBox->setChecked(false);
        ui->tabWidget->setTabEnabled(meshesTabIndex, false);
    }
    else if(game == "Custom")
    {
        logPath = "logs/Custom_log.html";
        iniPath = "settings/Custom/config.ini";
    }
}

void Custom::hideAdvancedSettings()
{
    ui->bsaAdvancedGroupBox->hide();
    ui->meshesAdvancedGroupBox->hide();
    ui->meshesVeryAdvancedGroupBox->hide();
    ui->texturesAdvancedGroupBox->hide();
    ui->animationsAdvancedGroupBox->hide();
}

void Custom::showAdvancedSettings()
{
    ui->bsaAdvancedGroupBox->show();
    ui->meshesAdvancedGroupBox->show();
    ui->meshesVeryAdvancedGroupBox->show();
    ui->texturesAdvancedGroupBox->show();
    ui->animationsAdvancedGroupBox->show();
}

Custom::~Custom()
{
    delete ui;
}
