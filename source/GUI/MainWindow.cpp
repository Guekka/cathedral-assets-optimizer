/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MainWindow.hpp"

namespace CAO {
MainWindow::MainWindow()
    : _ui(new ::Ui::MainWindow)
{
    _ui->setupUi(this);
    setAcceptDrops(true);

    //Connecting all settings changes to a variable
    {
        const auto &checkbox = this->findChildren<QCheckBox *>();
        const auto &radiobutton = this->findChildren<QRadioButton *>();
        const auto &lineEdit = this->findChildren<QLineEdit *>();
        const auto &list = this->findChildren<QListWidget *>();
        const auto &comboBox = this->findChildren<QComboBox *>();
        const auto &spinBox = this->findChildren<QDoubleSpinBox *>();
        const auto &groupBox = this->findChildren<QGroupBox *>();

        for (const auto &c : checkbox)
            connect(c, &QAbstractButton::pressed, this, [this] { this->_settingsChanged = true; });

        for (const auto &r : radiobutton)
            connect(r, &QAbstractButton::pressed, this, [this] { this->_settingsChanged = true; });

        for (const auto &l : lineEdit)
            connect(l, &QLineEdit::textEdited, this, [this] { this->_settingsChanged = true; });

        for (const auto &l : list)
            connect(l, &QListWidget::itemChanged, this, [this] { this->_settingsChanged = true; });

        for (const auto &c : comboBox)
            connect(c, QOverload<int>::of(&QComboBox::activated), this, [this] { this->_settingsChanged = true; });

        for (const auto &s : spinBox)
            connect(s, &QDoubleSpinBox::editingFinished, this, [this] { this->_settingsChanged = true; });

        for (const auto &g : groupBox)
            connect(g, &QGroupBox::toggled, this, [this] { this->_settingsChanged = true; });
    }

    //Setting data for widgets

    //Profiles
    refreshProfiles();
    {
        //Mode chooser combo box
        _ui->modeChooserComboBox->setItemData(0, SingleMod);
        _ui->modeChooserComboBox->setItemData(1, SeveralMods);

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
    }

    //Connecting widgets
    connect(_ui->dryRunCheckBox, &QCheckBox::clicked, this, [&](const bool &checked) {
        //Disabling BSA settings if dry run is enabled
        _ui->bsaBaseGroupBox->setDisabled(checked);
        _ui->bsaExtractCheckBox->setDisabled(checked);
        _ui->bsaCreateCheckbox->setDisabled(checked);
        _ui->bsaDeleteBackupsCheckBox->setDisabled(checked);

        _ui->bsaExtractCheckBox->setChecked(false);
        _ui->bsaCreateCheckbox->setChecked(false);
        _ui->bsaDeleteBackupsCheckBox->setChecked(false);
    });

    connect(_ui->advancedSettingsCheckbox, &QCheckBox::clicked, this, [&](const bool &enabled) {
        this->showTutorialWindow(tr("Advanced settings"),
                                 tr("Advanced settings can only be modified when using custom profiles."));
        this->setAdvancedSettingsEnabled(enabled);
    });

    disconnect(_ui->presets, nullptr, nullptr, nullptr); //resetting
    connect(_ui->presets, QOverload<int>::of(&QComboBox::activated), this, [&] {
        this->setGameMode(_ui->presets->currentText());
    });

    connect(_ui->newProfilePushButton, &QPushButton::pressed, this, &MainWindow::createProfile);

    connect(_ui->modeChooserComboBox, QOverload<int>::of(&QComboBox::activated), this, [&] {
        const bool &severalModsEnabled = (_ui->modeChooserComboBox->currentData() == SeveralMods);

        //Disabling some meshes settings when several mods mode is enabled
        _ui->meshesMediumOptimizationRadioButton->setDisabled(severalModsEnabled);
        _ui->meshesFullOptimizationRadioButton->setDisabled(severalModsEnabled);
        _ui->meshesNecessaryOptimizationRadioButton->setChecked(severalModsEnabled);

        if (severalModsEnabled)
        {
            this->showTutorialWindow(
                tr("Several mods option"),
                tr("You have selected the several mods option. This process may take a very long time, "
                   "especially if you process BSA. ")
                    + '\n' + tr("This process has only been tested on the Mod Organizer mods folder."));
        }
        _settingsChanged = true;
    });

    connect(_ui->userPathButton, &QPushButton::pressed, this, [&] {
        const QString &dir = QFileDialog::getExistingDirectory(this,
                                                               tr("Open Directory"),
                                                               _settings.getValue<QString>(sUserPath),
                                                               QFileDialog::ShowDirsOnly
                                                                   | QFileDialog::DontResolveSymlinks);
        if (!dir.isEmpty())
            _ui->userPathTextEdit->setText(dir);
        this->_settingsChanged = true;
    });

    connect(_ui->userPathTextEdit, &QLineEdit::textChanged, this, [this] { this->_settingsChanged = true; });

    connect(_ui->processButton, &QPushButton::pressed, this, &MainWindow::initProcess);

    bsaFilesToPackDialog = new BSAFilesToPackWidget(this);
    bsaFilesToPackDialog->hide();
    connect(_ui->bsaFilesToPackButton, &QPushButton::pressed, this, [this] {
        bsaFilesToPackDialog->show();
        _settingsChanged = true;
    });

    texturesFormatDialog = new ListDialog(this);
    for (const auto &value : Detail::DxgiFormats)
    {
        auto item = new QListWidgetItem(value.name);
        item->setData(Qt::UserRole, value.format);
        item->setCheckState(Qt::Unchecked);
        texturesFormatDialog->addItem(item);
    }
    texturesFormatDialog->setUserAddItemAllowed(false);

    connect(_ui->texturesUnwantedFormatsEditButton, &QPushButton::pressed, this, [&] {
        QStringList unwantedFormats;
        for (int i = 0; i < _ui->texturesUnwantedFormatsList->count(); ++i)
            unwantedFormats << _ui->texturesUnwantedFormatsList->item(i)->text();
        _ui->texturesUnwantedFormatsList->clear();

        texturesFormatDialog->setCheckedItems(unwantedFormats);
        texturesFormatDialog->open();
    });

    connect(texturesFormatDialog, &QDialog::finished, this, [&] {
        for (const auto &item : texturesFormatDialog->getChoices())
        {
            item->setFlags(item->flags() & (~Qt::ItemIsUserCheckable) & (~Qt::Checked));
            _ui->texturesUnwantedFormatsList->addItem(item);
            _settingsChanged = true;
        }
    });

    //Connecting menu buttons
    {
        connect(_ui->actionEnableDarkTheme, &QAction::triggered, this, &MainWindow::setDarkTheme);

        connect(_ui->actionShow_tutorials, &QAction::triggered, this, [this](const bool &checked) {
            this->_showTutorials = checked;
        });

        connect(_ui->actionEnable_debug_log, &QAction::triggered, this, [this] {
            this->_settingsChanged = true;
            this->saveUi();
        });

        connect(_ui->actionOpen_log_file, &QAction::triggered, this, [] {
            QDesktopServices::openUrl(QUrl("file:///" + Profiles::logPath(), QUrl::TolerantMode));
        });

        connect(_ui->actionAbout, &QAction::triggered, this, [&] {
            QMessageBox::about(this,
                               tr("About"),
                               QCoreApplication::applicationName() + ' ' + QCoreApplication::applicationVersion()
                                   + tr("\nMade by G'k\nThis program is distributed in the hope that it will be useful "
                                        "but WITHOUT ANY "
                                        "WARRANTLY. See the Mozilla Public License"));
        });
        connect(_ui->actionAbout_Qt, &QAction::triggered, this, [&] { QMessageBox::aboutQt(this); });

        connect(_ui->actionDocumentation, &QAction::triggered, this, [&] {
            QDesktopServices::openUrl(QUrl("https://www.nexusmods.com/skyrimspecialedition/mods/23316"));
        });

        connect(_ui->actionDiscord, &QAction::triggered, this, [&] {
            QDesktopServices::openUrl(QUrl("https://discordapp.com/invite/B9abN8d"));
        });

        connect(_ui->actionSave_UI, &QAction::triggered, this, [this] {
            saveUi();
            loadUi();
        });
    }

    //Loading remembered settings
    _settings.readFromUi(*_ui, bsaFilesToPackDialog->getUi());
    _settingsChanged = false;
    setGameMode(Profiles::currentProfile());
    firstStart();
    _settingsChanged = false;
}

void MainWindow::saveUi()
{
    Profiles::commonSettings()->setValue("bShowAdvancedSettings", _ui->advancedSettingsCheckbox->isChecked());
    Profiles::commonSettings()->setValue("bDarkMode", _ui->actionEnableDarkTheme->isChecked());
    Profiles::commonSettings()->setValue("alwaysSaveSettings", _alwaysSaveSettings);
    Profiles::commonSettings()->setValue("showTutorial", _showTutorials);

    if (!_settingsChanged)
    {
        //Restoring previous settings in case an unregistered change happened
        _settings.saveToUi(*_ui, bsaFilesToPackDialog->getUi());
        return;
    }

    if (!_alwaysSaveSettings)
    {
        QMessageBox box(QMessageBox::Information,
                        tr("Save unsaved changes"),
                        tr("You have unsaved changes. Do you want to save them?"),
                        QMessageBox::No | QMessageBox::Yes);

        QPushButton *alwaysButton = box.addButton(tr("Always"), QMessageBox::YesRole);
        box.exec();

        if (box.result() == QMessageBox::No)
        {
            _settings.saveToUi(*_ui, bsaFilesToPackDialog->getUi()); //Restoring previous settings
            return;
        }

        if (box.clickedButton() == alwaysButton)
        {
            _alwaysSaveSettings = true;
            saveUi();
        }
    }

    //If a box is unchecked, all sub-checkboxes have to be unchecked
    for (QGroupBox *widget : this->findChildren<QGroupBox *>())
    {
        if (!widget->isChecked() && widget->isCheckable())
            for (auto &box : widget->findChildren<QCheckBox *>())
                box->setChecked(false);
    }

    for (int i = 0; i < _ui->tabWidget->count(); ++i)
    {
        const QWidget *currentTab = _ui->tabWidget->widget(i);
        if (!currentTab->isEnabled())
        {
            for (auto &box : currentTab->findChildren<QCheckBox *>())
                box->setChecked(false);
            for (auto &box : currentTab->findChildren<QRadioButton *>())
                box->setChecked(false);
            for (auto &box : currentTab->findChildren<QGroupBox *>())
                box->setChecked(false);
        }
    }

    _settings.readFromUi(*_ui, bsaFilesToPackDialog->getUi());
    _settings.saveToJSON(Profiles::settingsPath());

    _settingsChanged = false;
}

void MainWindow::loadUi()
{
    setDarkTheme(Profiles::commonSettings()->value("bDarkMode").toBool());
    _ui->advancedSettingsCheckbox->setChecked(Profiles::commonSettings()->value("bShowAdvancedSettings").toBool());
    _ui->presets->setCurrentIndex(_ui->presets->findText(Profiles::commonSettings()->value("profile").toString()));
    _alwaysSaveSettings = Profiles::commonSettings()->value("alwaysSaveSettings").toBool();
    _showTutorials = Profiles::commonSettings()->value("showTutorial").toBool();
    _ui->actionShow_tutorials->setChecked(_showTutorials);

    _settings.readFromJSON(Profiles::settingsPath());
    _settings.saveToUi(*_ui, bsaFilesToPackDialog->getUi());

    _settingsChanged = false;

    //Loading settings does not enable groupboxes
    for (QGroupBox *widget : this->findChildren<QGroupBox *>())
    {
        if (widget->findChildren<QRadioButton *>().size())
            continue; //We do not want to disable boxes with radio buttons

        const auto &buttonList = widget->findChildren<QCheckBox *>();
        if (!buttonList.size())
            continue; //We do not want to disable boxes without pushButton

        const bool &boxesAreUnchecked = std::none_of(buttonList.cbegin(),
                                                     buttonList.cend(),
                                                     [](const QCheckBox *button) { return button->isChecked(); });

        widget->setChecked(!boxesAreUnchecked);
    }
}

void MainWindow::resetUi() const
{
    //Resetting the window
    for (int i = 0; i < _ui->tabWidget->count(); ++i)
        _ui->tabWidget->setTabEnabled(i, true);

    _ui->meshesFullOptimizationRadioButton->show();
    _ui->meshesMediumOptimizationRadioButton->show();
}

void MainWindow::readProgress(const QString &text, const int &max, const int &value) const
{
    _ui->progressBar->setFormat(text);
    _ui->progressBar->setMaximum(max);
    _ui->progressBar->setValue(value);
}

void MainWindow::refreshProfiles()
{
    _ui->presets->clear();
    _ui->presets->addItems(Profiles::list());
}

void MainWindow::createProfile()
{
    showTutorialWindow(tr("New profile"),
                       tr("You are about to create a new profile. It will create a new directory in 'CAO/profiles'. "
                          "Please check it out after creation, some files will be created inside it."));

    bool ok = false;
    const QString &text = QInputDialog::getText(this, tr("New profile"), tr("Name:"), QLineEdit::Normal, "", &ok);
    if (!ok || text.isEmpty())
        return;

    //Choosing base profile

    QStringList profilesList;
    for (int i = 0; i < _ui->presets->count(); ++i)
        profilesList << _ui->presets->itemText(i);

    const QString &baseProfile = QInputDialog::getItem(this,
                                                       tr("Base profile"),
                                                       tr("Which profile do you want to use as a base?"),
                                                       profilesList,
                                                       _ui->presets->currentIndex(),
                                                       false,
                                                       &ok);

    if (!ok)
        return;

    Profiles::create(text, baseProfile);
    refreshProfiles();
    _ui->presets->setCurrentIndex(_ui->presets->findText(text));
    setGameMode(text);
}

void MainWindow::setDarkTheme(const bool &enabled)
{
    _ui->actionEnableDarkTheme->setChecked(enabled);
    _settingsChanged = true;

    if (enabled)
    {
        QFile f(":qdarkstyle/style.qss");
        f.open(QFile::ReadOnly | QFile::Text);
        qApp->setStyleSheet(f.readAll());
        f.close();
    }
    else
        qApp->setStyleSheet("");
}

void MainWindow::initProcess()
{
    saveUi();
    _ui->processButton->setDisabled(true);
    _bLockVariables = true;

    try
    {
        _caoProcess.reset();
        _caoProcess = std::make_unique<Manager>(_settings);
        connect(&*_caoProcess, &Manager::progressBarTextChanged, this, &MainWindow::readProgress);
        connect(&*_caoProcess, &Manager::progressBarTextChanged, this, &MainWindow::updateLog);
        connect(&*_caoProcess, &Manager::end, this, &MainWindow::endProcess);
        QtConcurrent::run(&*_caoProcess, &Manager::runOptimization);
    }
    catch (const std::exception &e)
    {
        QMessageBox box(QMessageBox::Critical,
                        tr("Error"),
                        tr("An exception has been encountered and the process was forced to stop: ")
                            + QString(e.what()));
        box.exec();
        endProcess();
    }
}

void MainWindow::endProcess()
{
    _ui->processButton->setDisabled(false);
    _bLockVariables = false;

    if (_caoProcess)
        _caoProcess->disconnect();

    _ui->progressBar->setMaximum(100);
    _ui->progressBar->setValue(100);
    _ui->progressBar->setFormat(tr("Done"));
    updateLog();
}

void MainWindow::updateLog() const
{
    _ui->logTextEdit->clear();

    QFile log(Profiles::logPath());
    if (log.open(QFile::Text | QFile::ReadOnly))
    {
        QTextStream ts(&log);
        ts.setCodec(QTextCodec::codecForName("UTF-8"));
        _ui->logTextEdit->appendHtml(ts.readAll());
    }
}

void MainWindow::setGameMode(const QString &mode)
{
    if (_settingsChanged)
        saveUi();

    //Resetting the window
    resetUi();

    //Actually setting the window mode
    Profiles::setCurrentProfile(mode);
    _settings.readFromJSON(Profiles::settingsPath());
    _settings.saveToUi(*_ui, bsaFilesToPackDialog->getUi());
    loadUi();

    const int &animTabIndex = _ui->tabWidget->indexOf(_ui->AnimationsTab);
    const int &meshesTabIndex = _ui->tabWidget->indexOf(_ui->meshesTab);
    const int &bsaTabIndex = _ui->tabWidget->indexOf(_ui->bsaTab);
    const int &TexturesTabIndex = _ui->tabWidget->indexOf(_ui->texturesTab);

    _ui->tabWidget->setTabEnabled(animTabIndex, _settings.getValue<bool>(bAnimationsTabEnabled));
    _ui->tabWidget->setTabEnabled(meshesTabIndex, _settings.getValue<bool>(bMeshesTabEnabled));
    _ui->tabWidget->setTabEnabled(bsaTabIndex, _settings.getValue<bool>(bBSATabEnabled));
    _ui->tabWidget->setTabEnabled(TexturesTabIndex, _settings.getValue<bool>(bTexturesTabEnabled));

    setAdvancedSettingsEnabled(_ui->advancedSettingsCheckbox->isChecked());
}

void MainWindow::setAdvancedSettingsEnabled(const bool &value)
{
    QWidgetList advancedSettings = {_ui->bsaAdvancedGroupBox,
                                    _ui->bsaExpertGroupBox,
                                    _ui->meshesVeryAdvancedGroupBox,
                                    _ui->texturesAdvancedGroupBox,
                                    _ui->animationsAdvancedGroupBox};

    const bool readOnly = Profiles::isBaseProfile();
    for (auto &window : advancedSettings)
    {
        window->setVisible(value);
        window->setDisabled(readOnly);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (_settingsChanged)
        saveUi();
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *e)
{
    const QString &fileName = e->mimeData()->urls().at(0).toLocalFile();
    QDir dir;
    if (dir.exists(fileName))
    {
        _ui->userPathTextEdit->setText(QDir::cleanPath(fileName));
        _settingsChanged = true;
    }
}

void MainWindow::showTutorialWindow(const QString &title, const QString &text)
{
    if (_showTutorials)
        QMessageBox::information(this, title, text);
}

void MainWindow::firstStart()
{
    if (!Profiles::commonSettings()->value("notFirstStart").toBool())
    {
        QMessageBox(
            QMessageBox::Information,
            tr("Welcome to %1 %2").arg(QCoreApplication::applicationName(), QCoreApplication::applicationVersion()),
            tr("It appears you are running CAO for the first time. All options have tooltips explaining what they "
               "do. If you need help, you can also join us on Discord. A dark theme is also available."))
            .exec();

        Profiles::commonSettings()->setValue("notFirstStart", true);
    }
}

MainWindow::~MainWindow()
{
    delete _ui;
}
} // namespace CAO
