/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "MainWindow.h"

MainWindow::MainWindow()
    : _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);

    //Setting data for widgets
    //Games combo box
    for (int i = 0; i < _ui->presets->count(); ++i)
    {
        QString text = _ui->presets->itemText(i);
        Games::GameMode game = Games::stringToGame(text);
        _ui->presets->setItemData(i, game);
    }

    //Mode chooser combo box
    _ui->modeChooserComboBox->setItemData(0, OptionsCAO::SingleMod);
    _ui->modeChooserComboBox->setItemData(1, OptionsCAO::SeveralMods);

    //Advanced BSA
    _ui->bsaFormat->setItemData(0, baTES3);
    _ui->bsaTexturesFormat->setItemData(0, baTES3);
    _ui->bsaFormat->setItemData(1, baTES4);
    _ui->bsaTexturesFormat->setItemData(1, baTES4);
    _ui->bsaFormat->setItemData(2, baFO3);
    _ui->bsaTexturesFormat->setItemData(2, baFO3);
    _ui->bsaFormat->setItemData(3, baSSE);
    _ui->bsaTexturesFormat->setItemData(3, baSSE);
    _ui->bsaFormat->setItemData(4, baFO4);
    _ui->bsaTexturesFormat->setItemData(4, baFO4dds);
    //Advanced meshes
    _ui->meshesUser->setItemData(0, 11);
    _ui->meshesUser->setItemData(1, 12);

    _ui->meshesVersion->setItemData(0, V20_0_0_5);
    _ui->meshesVersion->setItemData(1, V20_2_0_7);

    _ui->meshesStream->setItemData(0, 82);
    _ui->meshesStream->setItemData(1, 83);
    _ui->meshesStream->setItemData(2, 100);
    _ui->meshesStream->setItemData(3, 130);

    _ui->texturesOutputFormat->setItemData(0, DXGI_FORMAT_BC7_UNORM);
    _ui->texturesOutputFormat->setItemData(1, DXGI_FORMAT_BC3_UNORM);
    _ui->texturesOutputFormat->setItemData(2, DXGI_FORMAT_BC1_UNORM);
    _ui->texturesOutputFormat->setItemData(3, DXGI_FORMAT_R8G8B8A8_UNORM);

    //Connecting widgets
    connect(_ui->dryRunCheckBox, &QCheckBox::clicked, this, [&] {
        //Disabling BSA options if dry run is enabled
        if (_ui->dryRunCheckBox->isChecked())
        {
            _ui->bsaBaseGroupBox->setChecked(false);
            _ui->bsaBaseGroupBox->setEnabled(false);
        }
        else
            _ui->bsaBaseGroupBox->setEnabled(true);

        this->refreshUi();
    });

    connect(_ui->userPathTextEdit, &QLineEdit::textChanged, this, &MainWindow::refreshUi);

    connect(_ui->advancedSettingsCheckbox, &QCheckBox::clicked, this, [&]() {
        this->hideAdvancedSettings();
        this->refreshUi();
    });

    connect(_ui->presets, QOverload<int>::of(&QComboBox::activated), this, [&] {
        CAO_SET_CURRENT_GAME(Games::uiToGame(_ui))
        Games::getInstance()->saveToUi(_ui);
        this->refreshUi();
    });

    connect(_ui->modeChooserComboBox, QOverload<int>::of(&QComboBox::activated), this, [&] {
        if (_ui->modeChooserComboBox->currentIndex() == 1)
        {
            QMessageBox warning(this);
            warning.setText(tr("You have selected the several mods option. This process may take a very long time, "
                               "especially if you process BSA. "
                               "\nThis process has only been tested on the Mod Organizer mods folder."));
            warning.exec();
        }

        this->refreshUi();
    });

    connect(_ui->userPathButton, &QPushButton::pressed, this, [&] {
        QString dir = QFileDialog::getExistingDirectory(this,
                                                        "Open Directory",
                                                        _settings->value("SelectedPath").toString(),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (!dir.isEmpty())
            _ui->userPathTextEdit->setText(dir);
        this->refreshUi();
    });

    connect(_ui->processButton, &QPushButton::pressed, this, [&] {
        if (QDir(_ui->userPathTextEdit->text()).exists())
            this->initProcess();
        else
            QMessageBox::critical(this,
                                  tr("Non existing path"),
                                  tr("This path does not exist. Process aborted."),
                                  QMessageBox::Ok);
    });

    //Connecting menu buttons
    connect(_ui->actionEnableDarkTheme, &QAction::triggered, this, &MainWindow::refreshUi);
    connect(_ui->actionEnable_debug_log, &QAction::triggered, this, &MainWindow::refreshUi);

    //Setting timer to refresh UI
    _timer = new QTimer(this);
    _timer->start(10000);
    connect(_timer, &QTimer::timeout, this, &MainWindow::updateLog);
    connect(_timer, &QTimer::timeout, this, &MainWindow::refreshUi);

    //Loading remembered settings
    hideAdvancedSettings();
    resetUi();

    _uiSettings = new QSettings("settings/common/config.ini", QSettings::IniFormat, this);
    Games::GameMode mode = _uiSettings->value("game").value<Games::GameMode>();
    if (mode == Games::Invalid)
    {
        _uiSettings->setValue("game", Games::Custom);
        mode = Games::Custom;
    }
    else if (mode == Games::Custom)
    {
        QSettings gamesSettings("settings/Custom/config.ini", QSettings::IniFormat);
        Games::getInstance()->readFromIni(&gamesSettings);
        Games::getInstance()->saveToUi(_ui);
    }

    CAO_SET_CURRENT_GAME(mode)
    _settings = new QSettings(Games::iniPath(), QSettings::IniFormat, this);

    loadUi();
    refreshUi();

    setGameMode(mode);
}

void MainWindow::refreshUi()
{
    //Disabling some meshes options when several mods mode is enabled
    const bool severalModsEnabled = (_ui->modeChooserComboBox->currentIndex() == 1);
    _ui->meshesMediumOptimizationRadioButton->setDisabled(severalModsEnabled);
    _ui->meshesFullOptimizationRadioButton->setDisabled(severalModsEnabled);
    _ui->meshesNecessaryOptimizationRadioButton->setChecked(severalModsEnabled);

    //Disabling BSA options if dry run is enabled
    const bool dryRunEnabled = _ui->dryRunCheckBox->isChecked();
    _ui->bsaExtractCheckBox->setDisabled(dryRunEnabled);
    _ui->bsaCreateCheckbox->setDisabled(dryRunEnabled);
    _ui->bsaDeleteBackupsCheckbox->setDisabled(dryRunEnabled);

    //Enabling dark mode
    if (_ui->actionEnableDarkTheme->isChecked())
    {
        QFile f(":qdarkstyle/style.qss");
        f.open(QFile::ReadOnly | QFile::Text);
        qApp->setStyleSheet(f.readAll());
        f.close();
    }
    else
        qApp->setStyleSheet("");

    //Enabling advanced options
    if (_ui->advancedSettingsCheckbox->isChecked())
    {
        this->showAdvancedSettings();
        this->setGameMode(Games::Custom);
    }
    else
        setGameMode(Games::uiToGame(_ui));

    if (!_bLockVariables)
        saveUi();

    loadUi();
}

void MainWindow::saveUi()
{
    _uiSettings->setValue("bShowAdvancedSettings", _ui->advancedSettingsCheckbox->isChecked());
    _uiSettings->setValue("bDarkMode", _ui->actionEnableDarkTheme->isChecked());

    _options.readFromUi(_ui);
    _options.saveToIni(_settings);

    if (_ui->advancedSettingsCheckbox->isChecked())
    {
        QSettings sets("settings/Custom/config.ini", QSettings::IniFormat, this);
        Games::getInstance()->readFromUi(_ui);
        Games::getInstance()->saveToIni(&sets);
    }
}

void MainWindow::loadUi()
{
    _ui->actionEnableDarkTheme->setChecked(_uiSettings->value("bDarkMode").toBool());
    _ui->advancedSettingsCheckbox->setChecked(_uiSettings->value("bShowAdvancedSettings").toBool());
    for (int i = 0; i < _ui->presets->count(); ++i)
    {
        if (_ui->presets->itemData(i) == Games::game())
        {
            _ui->presets->setCurrentIndex(i);
            break;
        }
    }

    _options.readFromIni(_settings);
    _options.saveToUi(_ui);

    if (_ui->advancedSettingsCheckbox->isChecked())
        Games::getInstance()->saveToUi(_ui);
}

void MainWindow::resetUi() const
{
    //Resetting the window
    const int animTabIndex = _ui->tabWidget->indexOf(_ui->AnimationsTab);
    const int meshesTabIndex = _ui->tabWidget->indexOf(_ui->meshesTab);
    _ui->tabWidget->setTabEnabled(animTabIndex, true);
    _ui->tabWidget->setTabEnabled(meshesTabIndex, true);

    _ui->meshesFullOptimizationRadioButton->show();
    _ui->meshesMediumOptimizationRadioButton->show();
}

void MainWindow::readProgress(const QString &text, const int &max, const int &value) const
{
    _ui->progressBar->setFormat(text);
    _ui->progressBar->setMaximum(max);
    _ui->progressBar->setValue(value);
}

void MainWindow::initProcess()
{
    refreshUi();
    _ui->processButton->setDisabled(true);
    _bLockVariables = true;

    try
    {
        _caoProcess.reset();
        _caoProcess = std::make_unique<Manager>(_options);
        connect(&*_caoProcess, &Manager::progressBarTextChanged, this, &MainWindow::readProgress);
        QtConcurrent::run(&*_caoProcess, &Manager::runOptimization);
    }
    catch (const std::exception &e)
    {
        QMessageBox box(QMessageBox::Critical,
                        tr("Error"),
                        tr("An exception has been encountered and the process was forced to stop: ")
                            + QString(e.what()));
        box.exec();
    }
    endProcess();
}

void MainWindow::endProcess()
{
    _ui->processButton->setDisabled(false);
    _bLockVariables = false;

    if (_caoProcess)
        disconnect(&*_caoProcess, &Manager::progressBarTextChanged, this, &MainWindow::readProgress);

    _ui->progressBar->setMaximum(100);
    _ui->progressBar->setValue(100);
    _ui->progressBar->setFormat(tr("Done"));
    updateLog();
}

void MainWindow::updateLog() const
{
    _ui->logTextEdit->clear();

    QFile log(Games::logPath());
    if (log.open(QFile::Text | QFile::ReadOnly))
    {
        QTextStream ts(&log);
        ts.setCodec(QTextCodec::codecForName("UTF-8"));
        _ui->logTextEdit->appendHtml(ts.readAll());
        log.close();
    }
}

void MainWindow::setGameMode(const Games::GameMode &mode) const
{
    //Resetting the window
    const int animTabIndex = _ui->tabWidget->indexOf(_ui->AnimationsTab);
    const int meshesTabIndex = _ui->tabWidget->indexOf(_ui->meshesTab);
    resetUi();

    //Actually setting the window mode
    CAO_SET_CURRENT_GAME(mode)

    switch (mode)
    {
        //Doing nothing for SSE : it is the default mode
    case Games::SSE: break;
    case Games::Custom: break;

    case Games::TES5:
        _ui->meshesMediumOptimizationRadioButton->setChecked(false);
        _ui->meshesMediumOptimizationRadioButton->hide();

        _ui->animationsNecessaryOptimizationCheckBox->setChecked(false);
        _ui->tabWidget->setTabEnabled(animTabIndex, false);
        break;

    case Games::FO4:
        _ui->animationsNecessaryOptimizationCheckBox->setChecked(false);
        _ui->tabWidget->setTabEnabled(animTabIndex, false);

        _ui->meshesGroupBox->setChecked(false);
        _ui->tabWidget->setTabEnabled(meshesTabIndex, false);
        break;

    case Games::Invalid: throw std::runtime_error("Invalid game: " + _ui->presets->currentText().toStdString());
    }
}

void MainWindow::hideAdvancedSettings() const
{
    _ui->bsaAdvancedGroupBox->hide();
    _ui->meshesAdvancedGroupBox->hide();
    _ui->meshesVeryAdvancedGroupBox->hide();
    _ui->texturesAdvancedGroupBox->hide();
    _ui->animationsAdvancedGroupBox->hide();
}

void MainWindow::showAdvancedSettings() const
{
    _ui->bsaAdvancedGroupBox->show();
    _ui->meshesAdvancedGroupBox->show();
    _ui->meshesVeryAdvancedGroupBox->show();
    _ui->texturesAdvancedGroupBox->show();
    _ui->animationsAdvancedGroupBox->show();
}

MainWindow::~MainWindow()
{
    delete _ui;
}
