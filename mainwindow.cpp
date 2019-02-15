#include "mainwindow.hpp"
#include "Optimiser.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow() : ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    optimizer = new Optimiser(ui->mw_log, ui->progressBar);

    optimizer->loadSettings();
    this->loadUIFromVars();
    ui->modeChooserComboBox->setCurrentIndex(optimizer->options.mode);

    //Connecting simple mode checkboxes to variables

    connect(ui->bsaOptCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->setBsaOptBool(state);
        this->loadUIFromVars();
    });

    connect(ui->textOptCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->setTextOptBool(state);
        this->loadUIFromVars();
    });

    connect(ui->meshOptCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->setNifOptBool(state);
        this->loadUIFromVars();
    });

    connect(ui->animOptCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->setAnimOptBool(state);
    });


    //Connecting expert mode checkboxes to variables

    connect(ui->renameBsaCheckbox, &QCheckBox::clicked, this, [=](bool state){

        if(state)
        {
            ui->createBsaCheckbox->setDisabled(false);

        }else
        {
            ui->createBsaCheckbox->setDisabled(true);
            ui->createBsaCheckbox->setChecked(false);
        }
    });

    if(!optimizer->options.renameBsa)
    {
        ui->createBsaCheckbox->setDisabled(true);
        ui->createBsaCheckbox->setChecked(false);
    }

    connect(ui->extractBsaCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.extractBsa = state;
    });


    connect(ui->renameBsaCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.renameBsa = state;
        optimizer->saveSettings();
    });


    connect(ui->createBsaCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimizer->options.createBsa = state;
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


    //Connecting the other widgets

    connect(ui->dryRunPushButton, &QPushButton::pressed, this, [=]()
    {
        QMessageBox warning(this);
        warning.setText(tr("You have selected to perform a dry run. No files will be modified, but BSAs will be extracted if that option was selected."));
        warning.setStandardButtons(QMessageBox::Button::Abort | QMessageBox::Button::Ok);
        warning.exec();

        if (warning.result() == 0x00000400)
        {
            ui->tabWidget->setCurrentIndex(0);
            optimizer->dryRun();
        }
    });

    connect(ui->modeChooserComboBox, QOverload<int>::of(&QComboBox::activated), this, [=](int index)
    {
        optimizer->options.mode = index;
        if(index == 1)
            QMessageBox::warning(this, tr("Several mods option"), tr("You have selected the several mods option. This process may take a very long time, especially if you process BSA. The program may look frozen, but it will work.\nThis process has only been tested on the Mod Organizer mods folder."), QMessageBox::Ok);
    });



    connect(ui->userPathTextEdit, &QPlainTextEdit::textChanged, this, [=](){
        optimizer->options.userPath = ui->userPathTextEdit->toPlainText();
    });


    connect(ui->userPathButton, &QPushButton::pressed, this, [=](){
        QString dir = QFileDialog::getExistingDirectory(this, "Open Directory", optimizer->options.userPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        ui->userPathTextEdit->setPlainText(dir);
        optimizer->options.userPath = dir;
    });

    connect(ui->processButton, &QPushButton::pressed, this, [=]()
    {
        optimizer->mainProcess();
    });



}
void MainWindow::loadUIFromVars()     //Apply the Optimiser settings to the checkboxes

{
    ui->userPathTextEdit->setPlainText(optimizer->options.userPath);

    ui->bsaOptCheckbox->setChecked(optimizer->getBsaOptBool());
    ui->textOptCheckbox->setChecked(optimizer->getTextOptBool());
    ui->meshOptCheckbox->setChecked(optimizer->getNifOptBool());
    ui->animOptCheckbox->setChecked(optimizer->getAnimOptBool());

    ui->extractBsaCheckbox->setChecked(optimizer->options.extractBsa);
    ui->renameBsaCheckbox->setChecked(optimizer->options.renameBsa);
    ui->createBsaCheckbox->setChecked(optimizer->options.createBsa);

    ui->bc7ConvCheckbox->setChecked(optimizer->options.bc7Conv);
    ui->tgaConvCheckbox->setChecked(optimizer->options.tgaConv);
    ui->nifscanTexturesCheckbox->setChecked(optimizer->options.nifscanTextures);

    ui->HardCrashingNifCheckbox->setChecked(optimizer->options.hardCrashingMeshes);
    ui->otherMeshesCheckBox->setChecked(optimizer->options.otherMeshes);
}
