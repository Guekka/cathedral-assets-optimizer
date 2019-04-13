#include "Mainwindow.h"
#include "MainOptimizer.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow() : ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Loading remembered settings
    settings = new QSettings("Cathedral Assets Optimizer.ini", QSettings::IniFormat, this);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "Cathedral Assets Optimizer.ini");

    if(!settings->contains("logLevel"))
        settings->setValue("logLevel", logLevelToInt(QLogger::LogLevel::Info));
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
        if(QDir(ui->userPathTextEdit->text()).exists())
            this->initProcess();
        else
            QMessageBox::critical(this, tr("Non existing path"), tr("This path does not exist. Process aborted."), QMessageBox::Ok);

    });

    //Connecting menu buttons

    connect(ui->actionSwitch_to_dark_theme, &QAction::triggered, this, [=]()
    {
        bDarkMode = !bDarkMode;
        this->saveUIToFile();
        this->loadUIFromFile();
    });


    connect(ui->actionLogVerbosityInfo, &QAction::triggered, this, [=](){this->settings->setValue("logLevel", logLevelToInt(QLogger::LogLevel::Info));});
    connect(ui->actionLogVerbosityNote, &QAction::triggered, this, [=](){this->settings->setValue("logLevel", logLevelToInt(QLogger::LogLevel::Note));});
    connect(ui->actionLogVerbosityTrace, &QAction::triggered, this, [=](){this->settings->setValue("logLevel", logLevelToInt(QLogger::LogLevel::Trace));});
    connect(ui->actionLogVerbosityWarning, &QAction::triggered, this, [=](){this->settings->setValue("logLevel", logLevelToInt(QLogger::LogLevel::Warning));});
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

    //BSA checkboxes

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

    //Textures radio buttons

    if(!ui->texturesGroupBox->isChecked())
    {
        settings->setValue("bTexturesNecessaryOptimization", false);
        settings->setValue("bTexturesFullOptimization", false);
    }
    else if(ui->TexturesFullOptimizationRadioButton->isChecked())
    {
        settings->setValue("bTexturesNecessaryOptimization", true);
        settings->setValue("bTexturesFullOptimization", true);
    }
    else if(ui->texturesGroupBox->isChecked())
    {
        settings->setValue("bTexturesNecessaryOptimization", true);
        settings->setValue("bTexturesFullOptimization", false);
    }

    //Meshes radio buttons

    if(ui->MeshesFullOptimizationRadioButton->isChecked())
    {
        settings->setValue("bMeshesNecessaryOptimization", true);
        settings->setValue("bMeshesMediumOptimization", true);
        settings->setValue("bMeshesFullOptimization", true);
    }
    else if(ui->MeshesMediumOptimizationRadioButton->isChecked())
    {
        settings->setValue("bMeshesNecessaryOptimization", true);
        settings->setValue("bMeshesMediumOptimization", true);
        settings->setValue("bMeshesFullOptimization", false);
    }
    else if(ui->meshesGroupBox->isChecked())
    {
        settings->setValue("bMeshesNecessaryOptimization", true);
        settings->setValue("bMeshesMediumOptimization", false);
        settings->setValue("bMeshesFullOptimization", false);
    }
    if(!ui->meshesGroupBox->isChecked())
    {
        settings->setValue("bMeshesNecessaryOptimization", false);
        settings->setValue("bMeshesMediumOptimization", false);
        settings->setValue("bMeshesFullOptimization", false);
    }

    //Animations

    if(ui->animationsGroupBox->isChecked())
        settings->setValue("bAnimationsOptimization", true);
    else
        settings->setValue("bAnimationsOptimization", false);

    //Dry run and mode
    settings->setValue("DryRun", ui->dryRunCheckBox->isChecked());

    if(ui->dryRunCheckBox->isChecked())
    {
        ui->BsaGroupBox->setChecked(false);
        ui->BsaGroupBox->setEnabled(false);
        settings->setValue("bBsaExtract", false);
        settings->setValue("bBsaCreate", false);
        settings->setValue("bBsaPackLooseFiles", false);
        settings->setValue("bBsaDeleteBackup", false);
        settings->setValue("bBsaSplitAssets", false);
    }
    else
        ui->BsaGroupBox->setEnabled(true);

    settings->setValue("mode", ui->modeChooserComboBox->currentIndex());
    settings->setValue("SelectedPath", QDir::cleanPath(ui->userPathTextEdit->text()));

    //GUI

    settings->setValue("bDarkMode", bDarkMode);
    settings->setValue("BsaGroupBox", ui->BsaGroupBox->isChecked());
    settings->setValue("texturesGroupBox", ui->texturesGroupBox->isChecked());
    settings->setValue("meshesGroupBox", ui->meshesGroupBox->isChecked());
    settings->setValue("animationsGroupBox", ui->animationsGroupBox->isChecked());

    this->loadUIFromFile();
}

void MainWindow::loadUIFromFile()//Apply the Optimiser settings to the checkboxes
{
    ui->userPathTextEdit->setText(settings->value("SelectedPath").toString());

    //Options

    ui->extractBsaCheckbox->setChecked(settings->value("bBsaExtract").toBool());
    ui->recreatetBsaCheckbox->setChecked(settings->value("bBsaCreate").toBool());
    ui->packExistingAssetsCheckbox->setChecked(settings->value("bBsaPackLooseFiles").toBool());
    ui->bsaDeleteBackupsCheckbox->setChecked(settings->value("bBsaDeleteBackup").toBool());
    ui->bsaSplitAssetsCheckBox->setChecked(settings->value("bBsaSplitAssets").toBool());

    ui->TexturesNecessaryOptimizationRadioButton->setChecked(settings->value("bTexturesNecessaryOptimization").toBool());
    ui->TexturesFullOptimizationRadioButton->setChecked(settings->value("bTexturesFullOptimization").toBool());

    ui->MeshesNecessaryOptimizationRadioButton->setChecked(settings->value("bMeshesNecessaryOptimization").toBool());
    ui->MeshesMediumOptimizationRadioButton->setChecked(settings->value("bMeshesMediumOptimization").toBool());
    ui->MeshesFullOptimizationRadioButton->setChecked(settings->value("bMeshesFullOptimization").toBool());

    ui->animationOptimizationRadioButton->setChecked(settings->value("bAnimationsOptimization").toBool());

    ui->modeChooserComboBox->setCurrentIndex(settings->value("mode").toInt());
    ui->dryRunCheckBox->setChecked(settings->value("DryRun").toBool());

    if(settings->value("bBsaCreate").toBool())
        ui->packExistingAssetsCheckbox->setDisabled(false);
    else
    {
        ui->packExistingAssetsCheckbox->setChecked(false);
        settings->setValue("bBsaPackLooseFiles", false);
        ui->packExistingAssetsCheckbox->setDisabled(true);
    }

    //GUI options

    bDarkMode = settings->value("bDarkMode").toBool();

    ui->BsaGroupBox->setChecked(settings->value("BsaGroupBox").toBool());
    ui->texturesGroupBox->setChecked(settings->value("texturesGroupBox").toBool());
    ui->meshesGroupBox->setChecked(settings->value("meshesGroupBox").toBool());
    ui->animationsGroupBox->setChecked(settings->value("animationsGroupBox").toBool());

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
        settings->setValue("bMeshesNecessaryOptimization", false);
        settings->setValue("bMeshesMediumOptimization", false);
        settings->setValue("bMeshesFullOptimization", false);
        settings->setValue("bMeshesHeadparts", false);
    }
    else
    {
        ui->MeshesMediumOptimizationRadioButton->setDisabled(!ui->meshesGroupBox->isChecked());
        ui->MeshesFullOptimizationRadioButton->setDisabled(!ui->meshesGroupBox->isChecked());
        settings->setValue("bMeshesHeadparts", true);
    }

}



void MainWindow::updateLog()
{
    ui->plainTextEdit->clear();

    QFile log("logs/log.html");
    log.open(QFile::Text | QFile::ReadOnly);

    QTextStream ts(&log);
    ts.setCodec(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    ui->plainTextEdit->appendHtml(ts.readAll());
    log.close();
}

MainWindow::~MainWindow()
{
    delete ui;
}



