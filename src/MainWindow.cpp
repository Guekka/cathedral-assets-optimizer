/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "MainWindow.h"

MainWindow::MainWindow() : ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Setting data for widgets
    //Games combo box
    for(int i = 0; i < ui->presets->count(); ++i)
    {
        QString text = ui->presets->itemText(i);
        Games::GameMode game = Games::stringToGame(text);
        ui->presets->setItemData(i, game);
    }

    //Mode chooser combo box
    ui->modeChooserComboBox->setItemData(0, OptionsCAO::singleMod);
    ui->modeChooserComboBox->setItemData(1, OptionsCAO::severalMods);

    //Advanced BSA
    ui->bsaFormat->setItemData(0, baTES3);
    ui->bsaTexturesFormat->setItemData(0, baTES3);
    ui->bsaFormat->setItemData(1, baTES4);
    ui->bsaTexturesFormat->setItemData(1, baTES4);
    ui->bsaFormat->setItemData(2, baFO3);
    ui->bsaTexturesFormat->setItemData(2, baFO3);
    ui->bsaFormat->setItemData(3, baSSE);
    ui->bsaTexturesFormat->setItemData(3, baSSE);
    ui->bsaFormat->setItemData(4, baFO4);
    ui->bsaTexturesFormat->setItemData(4, baFO4dds);
    //Advanced meshes
    ui->meshesUser->setItemData(0, 11);
    ui->meshesUser->setItemData(0, 12);

    ui->meshesVersion->setItemData(0, V20_0_0_5);
    ui->meshesVersion->setItemData(1, V20_2_0_7);

    ui->meshesStream->setItemData(0, 82);
    ui->meshesStream->setItemData(1, 83);
    ui->meshesStream->setItemData(2, 100);
    ui->meshesStream->setItemData(3, 130);

    ui->texturesOutputFormat->setItemData(0, "BC7_UNORM");
    ui->texturesOutputFormat->setItemData(1, "BC3_UNORM");
    ui->texturesOutputFormat->setItemData(2, "BC1_UNORM");
    ui->texturesOutputFormat->setItemData(3, "R8G8B8A8_UNORM");

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
    });

    connect(ui->userPathTextEdit, &QLineEdit::textChanged, this, &MainWindow::refreshUI);

    connect(ui->advancedSettingsCheckbox, &QCheckBox::clicked, this, [&]()
    {
        this->hideAdvancedSettings();
        this->refreshUI();
    });

    connect(ui->presets, QOverload<int>::of(&QComboBox::activated), this, [&]
    {
        CAO_SET_CURRENT_GAME(Games::uiToGame(ui))
                Games::getInstance()->saveToUi(ui);
        this->refreshUI();
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

    connect(ui->userPathButton, &QPushButton::pressed, this, [&]{
        QString dir = QFileDialog::getExistingDirectory(this, "Open Directory",
                                                        settings->value("SelectedPath").toString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if(!dir.isEmpty()) ui->userPathTextEdit->setText(dir);
        this->refreshUI();
    });

    connect(ui->processButton, &QPushButton::pressed, this, [&]
    {
        if(QDir(ui->userPathTextEdit->text()).exists())
            this->initProcess();
        else
            QMessageBox::critical(this, tr("Non existing path"), tr("This path does not exist. Process aborted."), QMessageBox::Ok);
    });

    //Connecting menu buttons
    connect(ui->actionEnableDarkTheme, &QAction::triggered, this, &MainWindow::refreshUI);

    connect(ui->actionLogVerbosityInfo, &QAction::triggered, this, [&](){this->settings->setValue("iLogLevel", 3);});
    connect(ui->actionLogVerbosityInfo, &QAction::triggered, this, &MainWindow::refreshUI);

    connect(ui->actionLogVerbosityNote, &QAction::triggered, this, [&](){this->settings->setValue("iLogLevel", 4);});
    connect(ui->actionLogVerbosityNote, &QAction::triggered, this, &MainWindow::refreshUI);

    connect(ui->actionLogVerbosityTrace, &QAction::triggered, this, [&](){this->settings->setValue("iLogLevel", 6);});
    connect(ui->actionLogVerbosityTrace, &QAction::triggered, this, &MainWindow::refreshUI);

    //Setting timer to refresh UI
    timer = new QTimer(this);
    timer->start(10000);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateLog);
    connect(timer, &QTimer::timeout, this, &MainWindow::refreshUI);

    //Loading remembered settings
    hideAdvancedSettings();
    resetUi();

    uiSettings = new QSettings("settings/common/config.ini", QSettings::IniFormat, this);
    Games::GameMode mode = uiSettings->value("game").value<Games::GameMode>();
    if(mode == Games::Invalid)
    {
        uiSettings->setValue("game", Games::Custom);
        mode = Games::Custom;
    }
    else if(mode == Games::Custom)
    {
        QSettings gamesSettings("settings/Custom/config.ini", QSettings::IniFormat);
        Games::getInstance()->readFromIni(&gamesSettings);
        Games::getInstance()->saveToUi(ui);
    }

    CAO_SET_CURRENT_GAME(mode)
    settings = new QSettings(CAO_INI_PATH, QSettings::IniFormat, this);

    loadUi();
    refreshUI();

    setGameMode(mode);
}

void MainWindow::refreshUI()
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

    //Enabling advanced options
    if(ui->advancedSettingsCheckbox->isChecked())
    {
        this->showAdvancedSettings();
        this->setGameMode(Games::Custom);
    }
    else
        setGameMode(Games::uiToGame(ui));

    if(!bLockVariables)
        saveUi();

    loadUi();
}

void MainWindow::saveUi()
{
    uiSettings->setValue("bShowAdvancedSettings", ui->advancedSettingsCheckbox->isChecked());
    uiSettings->setValue("bDarkMode", ui->actionEnableDarkTheme->isChecked());

    options.readFromUi(ui);
    options.saveToIni(settings);

    if(ui->advancedSettingsCheckbox->isChecked())
    {
        QSettings sets("settings/Custom/config.ini", QSettings::IniFormat, this);
        Games::getInstance()->readFromUi(ui);
        Games::getInstance()->saveToIni(&sets);
    }
}

void MainWindow::loadUi()
{
    ui->actionEnableDarkTheme->setChecked(uiSettings->value("bDarkMode").toBool());
    ui->advancedSettingsCheckbox->setChecked(uiSettings->value("bShowAdvancedSettings").toBool());
    for(int i = 0; i < ui->presets->count(); ++i)
    {
        if(ui->presets->itemData(i) == CAO_GET_CURRENT_GAME)
        {
            ui->presets->setCurrentIndex(i);
            break;
        }
    }

    options.readFromIni(settings);
    options.saveToUi(ui);
}

void MainWindow::resetUi()
{
    //Resetting the window
    const int animTabIndex =  ui->tabWidget->indexOf(ui->AnimationsTab);
    const int meshesTabIndex = ui->tabWidget->indexOf(ui->meshesTab);
    ui->tabWidget->setTabEnabled(animTabIndex, true);
    ui->tabWidget->setTabEnabled(meshesTabIndex, true);

    ui->texturesFullOptimizationRadioButton->show();
    ui->meshesMediumOptimizationRadioButton->show();

}

void MainWindow::readProgress(const QString& text, const int& max, const int& value)
{
    ui->progressBar->setFormat(text);
    ui->progressBar->setMaximum(max);
    ui->progressBar->setValue(value);
}

void MainWindow::initProcess()
{
    refreshUI();
    ui->processButton->setDisabled(true);
    bLockVariables = true;

    try {
        caoProcess.reset();
        caoProcess = std::make_unique<Manager>(options);
        connect(&*caoProcess, &Manager::progressBarTextChanged, this, &MainWindow::readProgress);
        QtConcurrent::run(&*caoProcess, &Manager::runOptimization);
    } catch (const std::exception& e) {
        QMessageBox box(QMessageBox::Critical, "Error",
                        "An exception has been encountered and the process was forced to stop: " + QString(e.what()));
        box.exec();
    }
    endProcess();
}

void MainWindow::endProcess()
{
    ui->processButton->setDisabled(false);
    bLockVariables = false;
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(100);
    ui->progressBar->setFormat("Done");
    updateLog();
}

void MainWindow::updateLog()
{
    ui->logTextEdit->clear();

    QFile log(CAO_LOG_PATH);
    if(log.open(QFile::Text | QFile::ReadOnly))
    {
        QTextStream ts(&log);
        ts.setCodec(QTextCodec::codecForName("UTF-8"));
        ui->logTextEdit->appendHtml(ts.readAll());
        log.close();
    }
}

void MainWindow::setGameMode(const Games::GameMode& mode)
{
    //Resetting the window
    const int animTabIndex =  ui->tabWidget->indexOf(ui->AnimationsTab);
    const int meshesTabIndex = ui->tabWidget->indexOf(ui->meshesTab);
    resetUi();

    //Actually setting the window mode
    CAO_SET_CURRENT_GAME(mode)

            switch (mode)
    {
        //Doing nothing for SSE : it is the default mode
        case Games::SSE: break;
        case Games::Custom: break;

        case Games::TES5:
            ui->texturesFullOptimizationRadioButton->setChecked(false);
            ui->texturesFullOptimizationRadioButton->hide();

            ui->meshesMediumOptimizationRadioButton->setChecked(false);
            ui->meshesMediumOptimizationRadioButton->hide();

            ui->animationsNecessaryOptimizationCheckBox->setChecked(false);
            ui->tabWidget->setTabEnabled(animTabIndex, false);
            break;

        case Games::FO4:
            ui->animationsNecessaryOptimizationCheckBox->setChecked(false);
            ui->tabWidget->setTabEnabled(animTabIndex, false);

            ui->meshesGroupBox->setChecked(false);
            ui->tabWidget->setTabEnabled(meshesTabIndex, false);
            break;

        case Games::Invalid:
            throw std::runtime_error("Invalid game: " + ui->presets->currentText().toStdString());
    }
}

void MainWindow::hideAdvancedSettings()
{
    ui->bsaAdvancedGroupBox->hide();
    ui->meshesAdvancedGroupBox->hide();
    ui->meshesVeryAdvancedGroupBox->hide();
    ui->texturesAdvancedGroupBox->hide();
    ui->animationsAdvancedGroupBox->hide();
}

void MainWindow::showAdvancedSettings()
{
    ui->bsaAdvancedGroupBox->show();
    ui->meshesAdvancedGroupBox->show();
    ui->meshesVeryAdvancedGroupBox->show();
    ui->texturesAdvancedGroupBox->show();
    ui->animationsAdvancedGroupBox->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
