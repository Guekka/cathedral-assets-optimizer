#include "mainwindow.hpp"
#include "Optimiser.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow() : ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    optimiser = new Optimiser(ui->mw_log, ui->progressBar);

    optimiser->loadSettings();
    this->loadAllCheckboxesFromVars();
    ui->modeChooserComboBox->setCurrentIndex(optimiser->options.mode);

    //Connecting simple mode checkboxes to variables

    connect(ui->bsaOptCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimiser->setBsaOptBool(state);
        this->loadAllCheckboxesFromVars();
    });

    connect(ui->textOptCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimiser->setTextOptBool(state);
        this->loadAllCheckboxesFromVars();
    });

    connect(ui->meshOptCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimiser->setNifOptBool(state);
        this->loadAllCheckboxesFromVars();
    });

    connect(ui->animOptCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimiser->setAnimOptBool(state);
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

    if(!optimiser->options.renameBsa)
    {
        ui->createBsaCheckbox->setDisabled(true);
        ui->createBsaCheckbox->setChecked(false);
    }

    connect(ui->extractBsaCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimiser->options.extractBsa = state;
    });


    connect(ui->renameBsaCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimiser->options.renameBsa = state;
        optimiser->saveSettings();
    });


    connect(ui->createBsaCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimiser->options.createBsa = state;
        optimiser->saveSettings();
    });


    connect(ui->bc7ConvCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimiser->options.bc7Conv = state;
        optimiser->saveSettings();
    });

    connect(ui->tgaConvCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimiser->options.tgaConv = state;
        optimiser->saveSettings();
    });

    connect(ui->nifscanTexturesCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimiser->options.nifscanTextures = state;
        optimiser->saveSettings();
    });

    connect(ui->HardCrashingNifCheckbox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimiser->options.hardCrashingMeshes = state;
        optimiser->saveSettings();
    });

    connect(ui->otherMeshesCheckBox, &QCheckBox::clicked, this, [=](bool state)
    {
        optimiser->options.otherMeshes = state;
        optimiser->saveSettings();
    });


    //Connecting the other widgets

    connect(ui->dryRunPushButton, &QPushButton::pressed, this, [=]()
    {
        QMessageBox warning(this);
        warning.setText(tr("You have selected the dry run option. No files will be affected. BSA will be extracted if the option is checked."));
        warning.setStandardButtons(QMessageBox::Button::Abort | QMessageBox::Button::Ok);
        warning.exec();

        if (warning.result() == 0x00000400)
        {
            ui->tabWidget->setCurrentIndex(0);
            optimiser->dryRun();
        }
    });

    connect(ui->modeChooserComboBox, QOverload<int>::of(&QComboBox::activated), this, [=](int index)
    {
        optimiser->options.mode = index;
        if(index == 1)
            QMessageBox::warning(this, tr("Several mods option"), tr("You have selected the several mods option. This process may take a very long time, especially if you process BSA. The program may look frozen, but it will work.\nThis process has only been tested on the Mod Organizer mods folder."), QMessageBox::Ok);
    });



    connect(ui->userPathTextEdit, &QPlainTextEdit::textChanged, this, [=](){
        optimiser->options.userPath = ui->userPathTextEdit->toPlainText();
    });


    connect(ui->userPathButton, &QPushButton::pressed, this, [=](){
        QString dir = QFileDialog::getExistingDirectory(this, "Open Directory", optimiser->options.userPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        ui->userPathTextEdit->setPlainText(dir);
        optimiser->options.userPath = dir;
    });

    connect(ui->processButton, &QPushButton::pressed, this, [=]()
    {
        optimiser->mainProcess();
    });



}
void MainWindow::loadAllCheckboxesFromVars()     //Apply the Optimiser settings to the checkboxes

{
    ui->bsaOptCheckbox->setChecked(optimiser->getBsaOptBool());
    ui->textOptCheckbox->setChecked(optimiser->getTextOptBool());
    ui->meshOptCheckbox->setChecked(optimiser->getNifOptBool());
    ui->animOptCheckbox->setChecked(optimiser->getAnimOptBool());

    ui->extractBsaCheckbox->setChecked(optimiser->options.extractBsa);
    ui->renameBsaCheckbox->setChecked(optimiser->options.renameBsa);
    ui->createBsaCheckbox->setChecked(optimiser->options.createBsa);

    ui->bc7ConvCheckbox->setChecked(optimiser->options.bc7Conv);
    ui->tgaConvCheckbox->setChecked(optimiser->options.tgaConv);
    ui->nifscanTexturesCheckbox->setChecked(optimiser->options.nifscanTextures);

    ui->HardCrashingNifCheckbox->setChecked(optimiser->options.hardCrashingMeshes);
    ui->otherMeshesCheckBox->setChecked(optimiser->options.otherMeshes);
}
