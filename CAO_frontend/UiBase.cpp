/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "UiBase.h"

UiBase::UiBase(const QString &logPath, const QString &iniPath) :
    m_logPath(logPath),
    m_iniPath(iniPath),
    ui(new Ui::UiBase)
{
    ui->setupUi(this);

    ui->plainTextEdit->setReadOnly(true);
    ui->plainTextEdit->setMaximumBlockCount(25);

    //Connecting the other widgets

    connect(ui->dryRunCheckBox, &QCheckBox::clicked, this, &UiBase::saveUIToFile);
    connect(ui->userPathTextEdit, &QLineEdit::textChanged, this, &UiBase::saveUIToFile);

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

    connect(ui->actionLogVerbosityInfo, &QAction::triggered, this, [&](){this->settings->setValue("iLogLevel", 3);});
    connect(ui->actionLogVerbosityInfo, &QAction::triggered, this, [&](){ loadUIFromFile(); });

    connect(ui->actionLogVerbosityNote, &QAction::triggered, this, [&](){this->settings->setValue("iLogLevel", 4);});
    connect(ui->actionLogVerbosityNote, &QAction::triggered, this, [&](){ loadUIFromFile(); });

    connect(ui->actionLogVerbosityTrace, &QAction::triggered, this, [&](){this->settings->setValue("iLogLevel", 6);});
    connect(ui->actionLogVerbosityTrace, &QAction::triggered, this, [&](){ loadUIFromFile(); });

    connect(ui->actionSelect_game, &QAction::triggered, GameSelector::getInstance(), [&]() {
        GameSelector::getInstance()->mainProcess(true); } );

    //Setting caoProcess

    caoProcess = new QProcess(this);
    caoProcess->setProgram("bin/Cathedral_Assets_Optimizer_back.exe");

    QObject::connect(caoProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [&]()
    {
        ui->processButton->setDisabled(false);
        bLockVariables = false;
        ui->progressBar->setMaximum(100);
        ui->progressBar->setValue(100);
        timer->stop();
    });

    //Setting timer to read progress from caoProcess
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [&]()
    {

    });
}

UiBase::~UiBase()
{
    delete ui;
}

void UiBase::initProcess()
{
    caoProcess->start();
    ui->processButton->setDisabled(true);
    bLockVariables = true;
}

void UiBase::readProgress()
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
        this->updateLog();
    }
}

void UiBase::setupSettings()
{
    //Loading remembered settings
    settings = new QSettings(m_iniPath, QSettings::IniFormat, this);

    if(!settings->contains("iLogLevel"))
        settings->setValue("iLogLevel", 4);
}

void UiBase::updateLog()
{
    ui->plainTextEdit->clear();

    QFile log(m_logPath);
    if(log.open(QFile::Text | QFile::ReadOnly))
    {
        QTextStream ts(&log);
        ts.setCodec(QTextCodec::codecForName("UTF-8"));
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
        ui->plainTextEdit->appendHtml(ts.readAll());
        log.close();
    }
}

void UiBase::closeEvent(QCloseEvent* event)
{
    caoProcess->kill();
    event->accept();
}
