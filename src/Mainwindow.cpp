#include "Mainwindow.h"
#include "ui_mainwindow.h"
#include "MainOptimizer.h"
#include "QLogger.h"

MainWindow::MainWindow() : ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Loading remembered settings
    settings = new QSettings("Cathedral Assets Optimizer.ini", QSettings::IniFormat, this);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "Cathedral Assets Optimizer.ini");

    if(!settings->contains("iLogLevel"))
        settings->setValue("iLogLevel",  QLogger::logLevelToInt(QLogger::LogLevel::Info));
    this->loadUIFromFile();

    //Preparing log

    ui->plainTextEdit->setReadOnly(true);
    ui->plainTextEdit->setMaximumBlockCount(25);
    updateLog();

    //Connecting checkboxes to file

    connect(ui->BsaGroupBox, &QGroupBox::clicked, this, &MainWindow::saveUIToFile);
    connect(ui->extractBsaCheckbox, &QCheckBox::clicked, this, &MainWindow::saveUIToFile);
    connect(ui->recreatetBsaCheckbox, &QCheckBox::clicked, this, &MainWindow::saveUIToFile);
    connect(ui->packExistingAssetsCheckbox, &QCheckBox::clicked, this, &MainWindow::saveUIToFile);
    connect(ui->bsaDeleteBackupsCheckbox, &QCheckBox::clicked, this, &MainWindow::saveUIToFile);
    connect(ui->bsaSplitAssetsCheckBox, &QCheckBox::clicked, this, &MainWindow::saveUIToFile);

    connect(ui->texturesGroupBox, &QGroupBox::clicked, this, &MainWindow::saveUIToFile);
    connect(ui->TexturesFullOptimizationRadioButton, &QCheckBox::clicked, this, &MainWindow::saveUIToFile);
    connect(ui->TexturesNecessaryOptimizationRadioButton, &QCheckBox::clicked, this, &MainWindow::saveUIToFile);

    connect(ui->meshesGroupBox, &QGroupBox::clicked, this, &MainWindow::saveUIToFile);
    connect(ui->MeshesNecessaryOptimizationRadioButton, &QCheckBox::clicked, this, &MainWindow::saveUIToFile);
    connect(ui->MeshesMediumOptimizationRadioButton, &QCheckBox::clicked, this, &MainWindow::saveUIToFile);
    connect(ui->MeshesFullOptimizationRadioButton, &QCheckBox::clicked, this, &MainWindow::saveUIToFile);

    connect(ui->animationsGroupBox, &QGroupBox::clicked, this, &MainWindow::saveUIToFile);
    connect(ui->animationOptimizationRadioButton, &QCheckBox::clicked, this, &MainWindow::saveUIToFile);

    //Connecting the other widgets

    connect(ui->dryRunCheckBox, &QCheckBox::clicked, this, &MainWindow::saveUIToFile);
    connect(ui->userPathTextEdit, &QLineEdit::textChanged, this, &MainWindow::saveUIToFile);

    connect(ui->modeChooserComboBox, QOverload<int>::of(&QComboBox::activated), this, [=]
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


    connect(ui->userPathButton, &QPushButton::pressed, this, [=](){
        QString dir = QFileDialog::getExistingDirectory(this, "Open Directory",
                                                        settings->value("SelectedPath").toString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        ui->userPathTextEdit->setText(dir);
        this->saveUIToFile();
    });

    connect(ui->processButton, &QPushButton::pressed, this, [=]()
    {
        if(QDir(ui->userPathTextEdit->text()).exists() && ui->userPathTextEdit->text().size() > 5 && !ui->userPathTextEdit->text().isEmpty())
            this->initProcess();
        else
            QMessageBox::critical(this, tr("Non existing path"), tr("This path does not exist or is shorter than 5 characters. Process aborted."), QMessageBox::Ok);

    });

    //Connecting menu buttons

    connect(ui->actionSwitch_to_dark_theme, &QAction::triggered, this, [=]()
    {
        bDarkMode = !bDarkMode;
        this->saveUIToFile();
    });


    connect(ui->actionLogVerbosityInfo, &QAction::triggered, this, [=](){this->settings->setValue("iLogLevel", QLogger::logLevelToInt(QLogger::LogLevel::Info));});
    connect(ui->actionLogVerbosityInfo, &QAction::triggered, this, &MainWindow::loadUIFromFile);

    connect(ui->actionLogVerbosityNote, &QAction::triggered, this, [=](){this->settings->setValue("iLogLevel", QLogger::logLevelToInt(QLogger::LogLevel::Note));});
    connect(ui->actionLogVerbosityNote, &QAction::triggered, this, &MainWindow::loadUIFromFile);

    connect(ui->actionLogVerbosityTrace, &QAction::triggered, this, [=](){this->settings->setValue("iLogLevel", QLogger::logLevelToInt(QLogger::LogLevel::Trace));});
    connect(ui->actionLogVerbosityTrace, &QAction::triggered, this, &MainWindow::loadUIFromFile);

    connect(ui->actionLogVerbosityWarning, &QAction::triggered, this, [=](){this->settings->setValue("iLogLevel", QLogger::logLevelToInt(QLogger::LogLevel::Warning));});
    connect(ui->actionLogVerbosityWarning, &QAction::triggered, this, &MainWindow::loadUIFromFile);
}


void MainWindow::initProcess()
{
    optimizer = new MainOptimizer();

    //Preparing thread

    workerThread = new QThread(this);
    thread()->setObjectName("WorkerThread");

    connect(workerThread, &QThread::started, optimizer, &MainOptimizer::mainProcess);
    connect(optimizer, &MainOptimizer::end, workerThread, &QThread::quit);
    connect(workerThread, &QThread::finished, this, [=]()
    {
        this->endProcess();
        ui->processButton->setDisabled(false);
        ui->progressBar->setValue(ui->progressBar->maximum());
        QMessageBox warning(this);
        warning.setText(tr("Completed. Please read the log to check if any errors occurred (displayed in red)."));
        warning.addButton(QMessageBox::Ok);
        warning.exec();
    });

    //Connecting Optimiser to progress bar

    connect(optimizer, &MainOptimizer::progressBarReset, this, [=](){ui->progressBar->reset();});
    connect(optimizer, &MainOptimizer::progressBarMaximumChanged, this, [=](int maximum){ui->progressBar->setMaximum(maximum);});

    connect(optimizer, &MainOptimizer::progressBarBusy, this, [=](){
        progressBarValue = ui->progressBar->value();
        ui->progressBar->setMaximum(0);
        ui->progressBar->setValue(0);
        updateLog();
    });

    connect(optimizer, &MainOptimizer::progressBarIncrease, this, [=]
    {
        ui->progressBar->setValue(progressBarValue + 1);
        progressBarValue = ui->progressBar->value();
        updateLog();
    });

    connect(optimizer, &MainOptimizer::updateLog, this, &MainWindow::updateLog);

    optimizer->moveToThread(workerThread);
    workerThread->start();
    workerThread->setPriority(QThread::HighestPriority);
    ui->processButton->setDisabled(true);
    bLockVariables = true;

}


void MainWindow::endProcess()
{
    bLockVariables = false;
    updateLog();
    delete optimizer;
    delete workerThread;
}


void MainWindow::saveUIToFile()
{
    if(bLockVariables)
        return;

    //BSA

    settings->beginGroup("BSA");

    settings->setValue("BsaGroupBox", ui->BsaGroupBox->isChecked());

    if(ui->BsaGroupBox->isChecked())
    {
        settings->setValue("bBsaExtract", ui->extractBsaCheckbox->isChecked());
        settings->setValue("bBsaCreate", ui->recreatetBsaCheckbox->isChecked());
        settings->setValue("bBsaPackLooseFiles", ui->packExistingAssetsCheckbox->isChecked());
        settings->setValue("bBsaDeleteBackup", ui->bsaDeleteBackupsCheckbox->isChecked());
        settings->setValue("bBsaSplitAssets", ui->bsaSplitAssetsCheckBox->isChecked());
    }
    else
    {
        settings->setValue("bBsaExtract", false);
        settings->setValue("bBsaCreate", false);
        settings->setValue("bBsaPackLooseFiles", false);
        settings->setValue("bBsaDeleteBackup", false);
        settings->setValue("bBsaSplitAssets", false);
    }

    //Disabling BSA options if dry run is enabled

    if(ui->dryRunCheckBox->isChecked())
    {
        settings->setValue("bBsaExtract", false);
        settings->setValue("bBsaCreate", false);
        settings->setValue("bBsaPackLooseFiles", false);
        settings->setValue("bBsaDeleteBackup", false);
        settings->setValue("bBsaSplitAssets", false);
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

    //Meshes

    settings->beginGroup("Meshes");

    if(ui->meshesGroupBox->isChecked())
        settings->setValue("iMeshesOptimizationLevel", 1);
    if(ui->MeshesMediumOptimizationRadioButton->isChecked())
        settings->setValue("iMeshesOptimizationLevel", 2);
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

void MainWindow::loadUIFromFile()//Apply the Optimiser settings to the checkboxes
{
    ui->userPathTextEdit->setText(settings->value("SelectedPath").toString());

    //BSA

    settings->beginGroup("BSA");
    ui->BsaGroupBox->setChecked(settings->value("BsaGroupBox").toBool());
    ui->extractBsaCheckbox->setChecked(settings->value("bBsaExtract").toBool());
    ui->recreatetBsaCheckbox->setChecked(settings->value("bBsaCreate").toBool());
    ui->packExistingAssetsCheckbox->setChecked(settings->value("bBsaPackLooseFiles").toBool());
    ui->bsaDeleteBackupsCheckbox->setChecked(settings->value("bBsaDeleteBackup").toBool());
    ui->bsaSplitAssetsCheckBox->setChecked(settings->value("bBsaSplitAssets").toBool());

    if(settings->value("bBsaCreate").toBool())
        ui->packExistingAssetsCheckbox->setDisabled(false);
    else
    {
        ui->packExistingAssetsCheckbox->setChecked(false);
        settings->setValue("bBsaPackLooseFiles", false);
        ui->packExistingAssetsCheckbox->setDisabled(true);
    }

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

    //Meshes

    settings->beginGroup("Meshes");
    switch(settings->value("iMeshesOptimizationLevel").toInt())
    {
    case 0: ui->meshesGroupBox->setChecked(false); break;
    case 1: ui->meshesGroupBox->setChecked(true);     ui->MeshesNecessaryOptimizationRadioButton->setChecked(true);  break;
    case 2: ui->meshesGroupBox->setChecked(true);     ui->MeshesMediumOptimizationRadioButton->setChecked(true); break;
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
    ui->actionLogVerbosityWarning->setChecked(false);

    switch (settings->value("iLogLevel").toInt())
    {
    case 0: ui->actionLogVerbosityTrace->setChecked(true); break;
    case 2: ui->actionLogVerbosityNote->setChecked(true); break;
    case 3: ui->actionLogVerbosityInfo->setChecked(true); break;
    case 4: ui->actionLogVerbosityWarning->setChecked(true); break;
    }

    //Enabling dark mode

    if(bDarkMode)
    {
        QFile f(":qdarkstyle/style.qss");
        f.open(QFile::ReadOnly | QFile::Text);
        qApp->setStyleSheet(f.readAll());
        f.close();
        ui->actionSwitch_to_dark_theme->setText(tr("Switch to light theme"));
    }
    else if(!bDarkMode)
    {
        qApp->setStyleSheet("");
        ui->actionSwitch_to_dark_theme->setText(tr("Switch to dark theme"));
    }

    //Disabling some meshes options when several mods mode is enabled

    if(settings->value("iMode").toInt() == 1)
    {
        ui->MeshesMediumOptimizationRadioButton->setDisabled(true);
        ui->MeshesFullOptimizationRadioButton->setDisabled(true);
        ui->MeshesNecessaryOptimizationRadioButton->setChecked(true);
    }
    else if(ui->meshesGroupBox->isChecked())
    {
        ui->MeshesMediumOptimizationRadioButton->setDisabled(false);
        ui->MeshesFullOptimizationRadioButton->setDisabled(false);
    }

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



void MainWindow::updateLog()
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

MainWindow::~MainWindow()
{
    delete ui;
}



