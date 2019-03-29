#include "Mainwindow.h"
#include "Optimiser.h"
#include "ui_mainwindow.h"
#include "Devmodeui.h"

MainWindow::MainWindow() : ui(new Ui::MainWindow), bDarkMode(true), bLockVariables(false)
{
    ui->setupUi(this);
    optimizer = new Optimiser();
    devmode  = new devModeUI(optimizer);

    //Loading remembered settings

    optimizer->loadSettings();
    this->loadSettings();
    this->loadUIFromVars();

    //Preparing thread

    workerThread = new QThread(this);
    thread()->setObjectName("WorkerThread");

    connect(workerThread, &QThread::started, optimizer, &Optimiser::mainProcess);
    connect(optimizer, &Optimiser::end, workerThread, &QThread::quit);
    connect(optimizer, &Optimiser::end, optimizer, &Optimiser::deleteLater);
    connect(workerThread, &QThread::finished, this, [=]()
    {
        ui->processButton->setDisabled(false);
        ui->progressBar->setValue(ui->progressBar->maximum());
        QMessageBox warning(this);
        warning.setText(tr("Completed. Please read the log to check if any errors occurred (displayed in red)."));
        warning.addButton(QMessageBox::Ok);
        warning.exec();
        qApp->quit(); //FIXME Restarting app shouldn't be necessary
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    });

    //Connecting Optimiser to progress bar

    connect(optimizer, &Optimiser::progressBarReset, this, [=](){ui->progressBar->reset();});
    connect(optimizer, &Optimiser::progressBarMaximumChanged, this, [=](int maximum){ui->progressBar->setMaximum(maximum);});

    connect(optimizer, &Optimiser::progressBarBusy, this, [=](){
        progressBarValue = ui->progressBar->value();
        ui->progressBar->setMaximum(0);
        ui->progressBar->setValue(0);
    });

    connect(optimizer, &Optimiser::progressBarIncrease, this, [=]
    {
        ui->progressBar->setValue(progressBarValue + 1);
        progressBarValue = ui->progressBar->value();
    });

    //Connecting checkboxes to optimizer variables

    connect(ui->BsaGroupBox, &QGroupBox::clicked, this, [=](){this->saveUIToVars();});

    connect(ui->extractBsaCheckbox, &QCheckBox::clicked, [=](){saveUIToVars();});
    connect(ui->recreatetBsaCheckbox, &QCheckBox::clicked, this, [=]{this->saveUIToVars();});
    connect(ui->packExistingAssetsCheckbox, &QCheckBox::clicked, this, [=]{this->saveUIToVars();});
    connect(ui->bsaDeleteBackupsCheckbox, &QCheckBox::clicked, this, [=](){this->saveUIToVars();});

    connect(ui->texturesGroupBox, &QGroupBox::clicked, this, [=](){this->saveUIToVars();});

    connect(ui->TexturesFullOptimizationRadioButton, &QCheckBox::clicked, this, [=]{this->saveUIToVars();});
    connect(ui->TexturesNecessaryOptimizationRadioButton, &QCheckBox::clicked, this, [=]{this->saveUIToVars();});

    connect(ui->meshesGroupBox, &QGroupBox::clicked, this, [=](){this->saveUIToVars();});

    connect(ui->MeshesNecessaryOptimizationRadioButton, &QCheckBox::clicked, this, [=]{this->saveUIToVars();});
    connect(ui->MeshesMediumOptimizationRadioButton, &QCheckBox::clicked, this, [=]{this->saveUIToVars();});
    connect(ui->MeshesFullOptimizationRadioButton, &QCheckBox::clicked, this, [=]{this->saveUIToVars();});

    connect(ui->animationsGroupBox, &QGroupBox::clicked, this, [=](){this->saveUIToVars();});

    connect(ui->animationOptimizationRadioButton, &QCheckBox::clicked, this, [=]{this->saveUIToVars();});

    connect(ui->dryRunCheckBox, &QCheckBox::clicked, this, [=](bool state)
    {
        if(state)
        {
            QMessageBox warning(this);
            warning.setText(tr("You have selected to perform a dry run. No files will be modified, but BSAs will be extracted if that option was selected."));
            warning.addButton(QMessageBox::Ok);
            warning.exec();
        }
        this->saveUIToVars();
    });

    //Connecting the other widgets

    connect(ui->modeChooserComboBox, QOverload<int>::of(&QComboBox::activated), this, [=]
    {
        if(ui->modeChooserComboBox->currentIndex() == 1)
        {
            QMessageBox warning(this);
            warning.setText(tr("You have selected the several mods option. This process may take a very long time, especially if you process BSA. The program may look frozen, but it will work.\nThis process has only been tested on the Mod Organizer mods folder."));
            warning.exec();
        }
        this->saveUIToVars();
    });

    connect(ui->userPathTextEdit, &QLineEdit::textChanged, this, [=](){this->saveUIToVars();});

    connect(ui->userPathButton, &QPushButton::pressed, this, [=](){
        QString dir = QFileDialog::getExistingDirectory(this, "Open Directory",
                                                        optimizer->options.userPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        ui->userPathTextEdit->setText(dir);
        this->saveUIToVars();
    });

    connect(ui->processButton, &QPushButton::pressed, this, [=]()
    {
        if(QDir(ui->userPathTextEdit->text()).exists())
        {
            optimizer->moveToThread(workerThread);
            workerThread->start();
            workerThread->setPriority(QThread::HighestPriority);
            ui->processButton->setDisabled(true);
            bLockVariables = true;
        }
        else
            QMessageBox::critical(this, tr("Non existing path"), tr("This path does not exist. Process aborted."), QMessageBox::Ok);

    });

    //Connecting menu buttons

    connect(ui->actionSwitch_to_dark_theme, &QAction::triggered, this, [=]()
    {
        bDarkMode = !bDarkMode;
        this->saveSettings();
        this->loadUIFromVars();
    });

    connect(ui->actionOpen_log, &QAction::triggered, this, [=]()
    {
        optimizer->saveSettings();
        devmode->show();
    });

    connect(ui->actionLogVerbosityInfo, &QAction::triggered, this, [=]()
    {
        optimizer->setLogLevel(QLogger::LogLevel::Info);
    });

    connect(ui->actionLogVerbosityNote, &QAction::triggered, this, [=]()
    {
        optimizer->setLogLevel(QLogger::LogLevel::Note);
    });

    connect(ui->actionLogVerbosityTrace, &QAction::triggered, this, [=]()
    {
        optimizer->setLogLevel(QLogger::LogLevel::Trace);
    });

    connect(ui->actionLogVerbosityWarning, &QAction::triggered, this, [=]()
    {
        optimizer->setLogLevel(QLogger::LogLevel::Warning);
    });
}


void MainWindow::saveUIToVars()
{
    if(bLockVariables)
        return;

    //BSA checkboxes

    if(ui->BsaGroupBox->isChecked())
    {
        optimizer->options.bBsaExtract = ui->extractBsaCheckbox->isChecked();
        optimizer->options.bBsaCreate = ui->recreatetBsaCheckbox->isChecked();
        optimizer->options.bBsaPackLooseFiles = ui->packExistingAssetsCheckbox->isChecked();
        optimizer->options.bBsaDeleteBackup = ui->bsaDeleteBackupsCheckbox->isChecked();
        optimizer->options.bBsaSplitAssets = ui->bsaSplitAssetsCheckBox->isChecked();
    }
    else
    {
        optimizer->options.bBsaExtract = false;
        optimizer->options.bBsaCreate = false;
        optimizer->options.bBsaPackLooseFiles = false;
        optimizer->options.bBsaDeleteBackup = false;
        optimizer->options.bBsaSplitAssets = false;
    }

    //Textures radio buttons

    if(!ui->texturesGroupBox->isChecked())
    {
        optimizer->options.bTexturesNecessaryOptimization = false;
        optimizer->options.bTexturesFullOptimization = false;
    }
    else if(ui->TexturesFullOptimizationRadioButton->isChecked())
    {
        optimizer->options.bTexturesNecessaryOptimization = true;
        optimizer->options.bTexturesFullOptimization = true;
    }
    else if(ui->texturesGroupBox->isChecked())
    {
        optimizer->options.bTexturesNecessaryOptimization = true;
        optimizer->options.bTexturesFullOptimization = false;
    }

    //Meshes radio buttons

    if(ui->MeshesFullOptimizationRadioButton->isChecked())
    {
        optimizer->options.bMeshesNecessaryOptimization = true;
        optimizer->options.bMeshesMediumOptimization = true;
        optimizer->options.bMeshesFullOptimization = true;

    }
    else if(ui->MeshesMediumOptimizationRadioButton->isChecked())
    {
        optimizer->options.bMeshesNecessaryOptimization = true;
        optimizer->options.bMeshesMediumOptimization = true;
        optimizer->options.bMeshesFullOptimization = false;
    }
    else if(ui->meshesGroupBox->isChecked())
    {
        optimizer->options.bMeshesNecessaryOptimization = true;
        optimizer->options.bMeshesMediumOptimization = false;
        optimizer->options.bMeshesFullOptimization = false;
    }
    if(!ui->meshesGroupBox->isChecked())
    {
        optimizer->options.bMeshesNecessaryOptimization = false;
        optimizer->options.bMeshesMediumOptimization = false;
        optimizer->options.bMeshesFullOptimization = false;
    }

    //Animations

    if(ui->animationsGroupBox->isChecked())
        optimizer->options.bAnimationsOptimization = true;
    else
        optimizer->options.bAnimationsOptimization = false;

    //Dry run

    optimizer->options.bDryRun = ui->dryRunCheckBox->isChecked();

    //mode chooser combo box

    optimizer->options.mode = ui->modeChooserComboBox->currentIndex();

    //Other widgets
    optimizer->options.userPath = ui->userPathTextEdit->text();

    optimizer->saveSettings();
    this->saveSettings();
    this->loadUIFromVars();
}

void MainWindow::loadUIFromVars()//Apply the Optimiser settings to the checkboxes
{
    ui->userPathTextEdit->setText(optimizer->options.userPath);

    //Options

    ui->extractBsaCheckbox->setChecked(optimizer->options.bBsaExtract);
    ui->recreatetBsaCheckbox->setChecked(optimizer->options.bBsaCreate);
    ui->packExistingAssetsCheckbox->setChecked(optimizer->options.bBsaPackLooseFiles);
    ui->bsaDeleteBackupsCheckbox->setChecked(optimizer->options.bBsaDeleteBackup);

    ui->TexturesNecessaryOptimizationRadioButton->setChecked(optimizer->options.bTexturesNecessaryOptimization);
    ui->TexturesFullOptimizationRadioButton->setChecked(optimizer->options.bTexturesFullOptimization);

    ui->MeshesNecessaryOptimizationRadioButton->setChecked(optimizer->options.bMeshesNecessaryOptimization);
    ui->MeshesMediumOptimizationRadioButton->setChecked(optimizer->options.bMeshesMediumOptimization);
    ui->MeshesFullOptimizationRadioButton->setChecked(optimizer->options.bMeshesFullOptimization);

    ui->animationOptimizationRadioButton->setChecked(optimizer->options.bAnimationsOptimization);

    ui->modeChooserComboBox->setCurrentIndex(optimizer->options.mode);
    ui->dryRunCheckBox->setChecked(optimizer->options.bDryRun);

    if(optimizer->options.bBsaCreate)
    {
        ui->packExistingAssetsCheckbox->setDisabled(false);
    }
    else
    {
        ui->packExistingAssetsCheckbox->setChecked(false);
        optimizer->options.bBsaPackLooseFiles = false;
        ui->packExistingAssetsCheckbox->setDisabled(true);
    }


    //Dark mode

    if(bDarkMode)
    {
        QFile f(":qdarkstyle/style.qss");
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
        f.close();
        bDarkMode = true;
        ui->actionSwitch_to_dark_theme->setText(tr("Switch to light theme"));
    }
    else if(!bDarkMode)
    {
        qApp->setStyleSheet("");
        bDarkMode=false;
        ui->actionSwitch_to_dark_theme->setText(tr("Switch to dark theme"));
    }

    //Disabling some meshes options when several mods mode is enabled

    if(ui->modeChooserComboBox->currentIndex() == 1)
    {
        ui->MeshesMediumOptimizationRadioButton->setDisabled(true);
        ui->MeshesFullOptimizationRadioButton->setDisabled(true);
        ui->MeshesNecessaryOptimizationRadioButton->setChecked(true);
        optimizer->options.bMeshesFullOptimization = false;
        optimizer->options.bMeshesMediumOptimization = false;
        optimizer->options.bMeshesHeadparts = false;
    }
    else
    {
        ui->MeshesMediumOptimizationRadioButton->setDisabled(!ui->meshesGroupBox->isChecked());
        ui->MeshesFullOptimizationRadioButton->setDisabled(!ui->meshesGroupBox->isChecked());
        optimizer->options.bMeshesHeadparts = true;
    }
}

void MainWindow::saveSettings() //Saves settings to an ini file
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");

    settings.setValue("bDarkMode", bDarkMode);

    settings.setValue("BsaGroupBox", ui->BsaGroupBox->isChecked());
    settings.setValue("texturesGroupBox", ui->texturesGroupBox->isChecked());
    settings.setValue("meshesGroupBox", ui->meshesGroupBox->isChecked());
    settings.setValue("animationsGroupBox", ui->animationsGroupBox->isChecked());

}

void MainWindow::loadSettings() //Loads settings from the ini file
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");

    bDarkMode = settings.value("bDarkMode").toBool();

    ui->BsaGroupBox->setChecked(settings.value("BsaGroupBox").toBool());
    ui->texturesGroupBox->setChecked(settings.value("texturesGroupBox").toBool());
    ui->meshesGroupBox->setChecked(settings.value("meshesGroupBox").toBool());
    ui->animationsGroupBox->setChecked(settings.value("animationsGroupBox").toBool());
}

MainWindow::~MainWindow()
{
    delete devmode;
    delete ui;
}
