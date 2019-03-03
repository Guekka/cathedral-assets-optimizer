#include "mainwindow.hpp"
#include "Optimiser.hpp"
#include "ui_mainwindow.h"
#include "devmodeui.h"

MainWindow::MainWindow() : ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    optimizer = new Optimiser(ui->mw_log, ui->mw_log, ui->progressBar, false);
    devmode  = new devModeUI(optimizer);

    this->loadSettings();
    optimizer->loadSettings();
    this->loadUIFromVars();

    //Connecting checkboxes to variables

    connect(ui->extractBsaCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.extractBsa = state;
    });

    connect(ui->recreatetBsaCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.recreateBsa = state;
    });


    connect(ui->packExistingAssetsCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.packExistingFiles = state;
        optimizer->saveSettings();
    });


    connect(ui->bc7ConvCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.bc7Conv = state;
        optimizer->saveSettings();
    });

    connect(ui->tgaConvCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.tgaConv = state;
        optimizer->saveSettings();
    });

    connect(ui->nifscanTexturesCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.nifscanTextures = state;
        optimizer->saveSettings();
    });

    connect(ui->HardCrashingNifCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.hardCrashingMeshes = state;
        optimizer->saveSettings();
    });

    connect(ui->otherMeshesCheckBox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.otherMeshes = state;
        optimizer->saveSettings();
    });


    connect(ui->animOptCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.animOptBool = state;
        optimizer->saveSettings();
    });

    connect(ui->dryRunCheckBox, &QCheckBox::clicked, this, [=](bool state)
    {
        if(state)
        {
            QMessageBox warning(this);
            warning.setText(tr("You have selected to perform a dry run. No files will be modified, but BSAs will be extracted if that option was selected."));
            warning.setStandardButtons(QMessageBox::Button::Abort | QMessageBox::Button::Ok);
            warning.exec();

            if (warning.result() == 0x00000400)
            {
                optimizer->options.dryRun = true;
                optimizer->saveSettings();
                this->loadUIFromVars();
            }
            else
            {
                optimizer->options.dryRun = false;
                optimizer->saveSettings();
                this->loadUIFromVars();
            }
        }
        else
        {
            optimizer->options.dryRun = false;
            optimizer->saveSettings();
            this->loadUIFromVars();
        }

    });

    //Connecting the other widgets

    connect(ui->modeChooserComboBox, QOverload<int>::of(&QComboBox::activated), this, [=](int index)
    {
        optimizer->options.mode = index;
        if(index == 1)
            QMessageBox::warning(this, tr("Several mods option"), tr("You have selected the several mods option. This process may take a very long time, especially if you process BSA. The program may look frozen, but it will work.\nThis process has only been tested on the Mod Organizer mods folder."), QMessageBox::Ok);
    });



    connect(ui->userPathTextEdit, &QLineEdit::textChanged, this, [=](){
        optimizer->options.userPath = ui->userPathTextEdit->text();
        this->loadUIFromVars();
    });


    connect(ui->userPathButton, &QPushButton::pressed, this, [=](){
        QString dir = QFileDialog::getExistingDirectory(this, "Open Directory", optimizer->options.userPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        ui->userPathTextEdit->setText(dir);
        optimizer->options.userPath = dir;
    });

    connect(ui->processButton, &QPushButton::pressed, this, [=]()
    {
        if(QDir(ui->userPathTextEdit->text()).exists())
            optimizer->mainProcess();
        else
            QMessageBox::critical(this, tr("Non existing path"), tr("This path does not exist. Process aborted."), QMessageBox::Ok);

    });



    //Connecting menu buttons


    connect(ui->actionReset_to_default_settings, &QAction::triggered, this, [=]()
    {
        simpleMode=true;
        optimizer->resetToDefaultSettings();
        this->loadUIFromVars();
    });

    connect(ui->actionShow_advanced_settings, &QAction::triggered, this, [=]()
    {
        simpleMode=false;
        this->loadUIFromVars();
    });

    connect(ui->actionSwitch_to_dark_theme, &QAction::triggered, this, [=]()
    {
        darkMode = !darkMode;
        this->loadUIFromVars();
    });

    connect(ui->actionOpen_log, &QAction::triggered, this, [=]()
    {
        optimizer->saveSettings();
        devmode->show();
    });

}


void MainWindow::loadUIFromVars()     //Apply the Optimiser settings to the checkboxes

{
    ui->userPathTextEdit->setText(optimizer->options.userPath);

    if(simpleMode)
    {
        simpleMode = false;
        ui->processButton->setText("Run with default settings");
        ui->BsaGroupBox->hide();
        ui->animGroupBox->hide();
        ui->meshesGroupBox->hide();
        ui->texturesGroupBox->hide();

    }
    else
    {
        ui->processButton->setText("Run");
        ui->BsaGroupBox->show();
        ui->animGroupBox->show();
        ui->meshesGroupBox->show();
        ui->texturesGroupBox->show();
    }

    ui->extractBsaCheckbox->setChecked(optimizer->options.extractBsa);
    ui->recreatetBsaCheckbox->setChecked(optimizer->options.recreateBsa);
    ui->packExistingAssetsCheckbox->setChecked(optimizer->options.packExistingFiles);

    ui->bc7ConvCheckbox->setChecked(optimizer->options.bc7Conv);
    ui->tgaConvCheckbox->setChecked(optimizer->options.tgaConv);
    ui->nifscanTexturesCheckbox->setChecked(optimizer->options.nifscanTextures);

    ui->HardCrashingNifCheckbox->setChecked(optimizer->options.hardCrashingMeshes);
    ui->otherMeshesCheckBox->setChecked(optimizer->options.otherMeshes);

    ui->animOptCheckbox->setChecked(optimizer->options.animOptBool);

    ui->modeChooserComboBox->setCurrentIndex(optimizer->options.mode);

    ui->dryRunCheckBox->setChecked(optimizer->options.dryRun);

    if(darkMode)
    {
        QFile f(":qdarkstyle/style.qss");
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
        darkMode = true;
        ui->actionSwitch_to_dark_theme->setText(tr("Switch to light theme"));
    }
    else if(!darkMode)
    {
        qApp->setStyleSheet("");
        darkMode=false;
        ui->actionSwitch_to_dark_theme->setText(tr("Switch to dark theme"));
    }

    saveSettings();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    delete optimizer;
    delete  devmode;
    event->accept();
}



void MainWindow::saveSettings() //Saves settings to an ini file
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");

    settings.setValue("darkMode", darkMode);
    settings.setValue("simpleMode", simpleMode);
}


void MainWindow::loadSettings() //Loads settings from the ini file
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");

    darkMode = settings.value("darkMode").toBool();
    simpleMode = settings.value("simpleMode").toBool();
}
