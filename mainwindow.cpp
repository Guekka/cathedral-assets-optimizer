#include "mainwindow.hpp"
#include "Optimiser.hpp"
#include "ui_mainwindow.h"
#include "devmodeui.h"

MainWindow::MainWindow() : ui(new Ui::MainWindow)
//TODO Allows the log to be saved to a file
//TODO Disable pack loose assets checkbox when create bsa is unchecked

{
    ui->setupUi(this);
    optimizer = new Optimiser(ui->mw_log, ui->mw_log, ui->progressBar);
    devmode  = new devModeUI(optimizer);

    //Loading remembered settings

    this->loadSettings();
    optimizer->loadSettings();
    this->loadUIFromVars();

    //Connecting checkboxes to optimizer variables

    connect(ui->extractBsaCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.bExtractBsa = state;
    });

    connect(ui->recreatetBsaCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.bCreateBsa = state;
    });


    connect(ui->packExistingAssetsCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.bPackExistingFiles = state;
        optimizer->saveSettings();
    });


    connect(ui->bc7ConvCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.bBc7Conversion = state;
        optimizer->saveSettings();
    });

    connect(ui->tgaConvCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.bTgaConversion = state;
        optimizer->saveSettings();
    });

    connect(ui->nifscanTexturesCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.bNifscanOnTextures = state;
        optimizer->saveSettings();
    });

    connect(ui->HardCrashingNifCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.bOptimizeHardCrashingMeshes = state;
        optimizer->saveSettings();
    });

    connect(ui->otherMeshesCheckBox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.bOptimizeOtherMeshes = state;
        optimizer->saveSettings();
    });


    connect(ui->animOptCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.bOptimizeAnimations = state;
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
                optimizer->options.bDryRun = true;
                optimizer->saveSettings();
                this->loadUIFromVars();
            }
            else
            {
                optimizer->options.bDryRun = false;
                optimizer->saveSettings();
                this->loadUIFromVars();
            }
        }
        else
        {
            optimizer->options.bDryRun = false;
            optimizer->saveSettings();
            this->loadUIFromVars();
        }

    });

    //Connecting the other widgets

    connect(ui->modeChooserComboBox, QOverload<int>::of(&QComboBox::activated), this, [=](int index)
    {
        optimizer->options.mode = index;
        if(index == 1)
            QMessageBox::warning(this, tr("Several mods option"),
                                 tr("You have selected the several mods option. This process may take a very long time, especially if you process BSA. The program may look frozen, but it will work.\nThis process has only been tested on the Mod Organizer mods folder."),
                                 QMessageBox::Ok);
    });



    connect(ui->userPathTextEdit, &QLineEdit::textChanged, this, [=](){
        optimizer->options.userPath = ui->userPathTextEdit->text();
        this->loadUIFromVars();
    });


    connect(ui->userPathButton, &QPushButton::pressed, this, [=](){
        QString dir = QFileDialog::getExistingDirectory(this, "Open Directory",
                                                        optimizer->options.userPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
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
        bSimpleMode=true;
        optimizer->resetToDefaultSettings();
        this->loadUIFromVars();
    });

    connect(ui->actionShow_advanced_settings, &QAction::triggered, this, [=]()
    {
        bSimpleMode=false;
        this->loadUIFromVars();
    });

    connect(ui->actionSwitch_to_dark_theme, &QAction::triggered, this, [=]()
    {
        bDarkMode = !bDarkMode;
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

    if(bSimpleMode)
    {
        bSimpleMode = false;
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

    ui->extractBsaCheckbox->setChecked(optimizer->options.bExtractBsa);
    ui->recreatetBsaCheckbox->setChecked(optimizer->options.bCreateBsa);
    ui->packExistingAssetsCheckbox->setChecked(optimizer->options.bPackExistingFiles);

    ui->bc7ConvCheckbox->setChecked(optimizer->options.bBc7Conversion);
    ui->tgaConvCheckbox->setChecked(optimizer->options.bTgaConversion);
    ui->nifscanTexturesCheckbox->setChecked(optimizer->options.bNifscanOnTextures);

    ui->HardCrashingNifCheckbox->setChecked(optimizer->options.bOptimizeHardCrashingMeshes);
    ui->otherMeshesCheckBox->setChecked(optimizer->options.bOptimizeOtherMeshes);

    ui->animOptCheckbox->setChecked(optimizer->options.bOptimizeAnimations);

    ui->modeChooserComboBox->setCurrentIndex(optimizer->options.mode);

    ui->dryRunCheckBox->setChecked(optimizer->options.bDryRun);

    if(bDarkMode)
    {
        QFile f(":qdarkstyle/style.qss");
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
        bDarkMode = true;
        ui->actionSwitch_to_dark_theme->setText(tr("Switch to light theme"));
    }
    else if(!bDarkMode)
    {
        qApp->setStyleSheet("");
        bDarkMode=false;
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

    settings.setValue("darkMode", bDarkMode);
    settings.setValue("simpleMode", bSimpleMode);
}


void MainWindow::loadSettings() //Loads settings from the ini file
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");

    bDarkMode = settings.value("darkMode").toBool();
    bSimpleMode = settings.value("simpleMode").toBool();
}
