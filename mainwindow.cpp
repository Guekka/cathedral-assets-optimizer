#include "mainwindow.hpp"
#include "Optimiser.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow() : ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    optimiser = new Optimiser("C:/", ui->mw_log, ui->progressBar);
    optimiser->loadSettings();

    ui->bsaOptCheckbox->setChecked(optimiser->getBsaOptBool());


    ui->extractBsaCheckbox->setChecked(optimiser->getExtractBsaBool());
    ui->renameBsaCheckbox->setChecked(optimiser->getrenameBsaBool());
    ui->createBsaCheckbox->setChecked(optimiser->getCreateBsaBool());

    if(!ui->renameBsaCheckbox->isChecked())
    {
        ui->createBsaCheckbox->setEnabled(false);
        ui->createBsaCheckbox->setChecked(false);
    }

    ui->textOptCheckbox->setChecked(optimiser->getTextOptBool());
    ui->meshOptCheckbox->setChecked(optimiser->getNifOptBool());
    ui->animOptCheckbox->setChecked(optimiser->getAnimOptBool());



    connect(ui->userPathTextEdit, &QPlainTextEdit::textChanged, this, [=](){
        optimiser->setUserPath(ui->userPathTextEdit->toPlainText());
    });


    connect(ui->userPathButton, &QPushButton::pressed, this, [=](){
        QString dir = QFileDialog::getExistingDirectory(this, "Open Directory", optimiser->getUserPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        ui->userPathTextEdit->setPlainText(dir);
        optimiser->setUserPath(dir);
    });

    connect(ui->renameBsaCheckbox, &QCheckBox::pressed, this, [=](){
        if(ui->renameBsaCheckbox->isChecked())
        {
            ui->createBsaCheckbox->setDisabled(true);
            ui->createBsaCheckbox->setChecked(false);
        }else
        {
            ui->createBsaCheckbox->setDisabled(false);
        }
    });

    connect(ui->extractBsaCheckbox, &QCheckBox::pressed, this, [=]()
    {
        optimiser->setExtractBsaBool(ui->extractBsaCheckbox->isChecked());
    });


    connect(ui->renameBsaCheckbox, &QCheckBox::pressed, this, [=]()
    {
        optimiser->setRenameBsaBool(ui->renameBsaCheckbox->isChecked());
    });

    connect(ui->bsaOptCheckbox, &QCheckBox::pressed, this, [=]()
    {
        optimiser->setBsaOptBool(ui->bsaOptCheckbox->isChecked());
        ui->extractBsaCheckbox->setChecked(optimiser->getExtractBsaBool());
        ui->renameBsaCheckbox->setChecked(optimiser->getrenameBsaBool());
        ui->createBsaCheckbox->setChecked(optimiser->getCreateBsaBool());
    });

    connect(ui->textOptCheckbox, &QCheckBox::pressed, this, [=]()
    {
        optimiser->setTextOptBool(ui->textOptCheckbox->isChecked());
    });

    connect(ui->meshOptCheckbox, &QCheckBox::pressed, this, [=]()
    {
        optimiser->setNifOptBool(ui->meshOptCheckbox->isChecked());
    });

    connect(ui->animOptCheckbox, &QCheckBox::pressed, this, [=]()
    {
        optimiser->setAnimOptBool(ui->animOptCheckbox->isChecked());
    });

    connect(ui->createBsaCheckbox, &QCheckBox::pressed, this, [=]()
    {
        optimiser->setCreateBsaBool(ui->createBsaCheckbox->isChecked());
    });

    connect(ui->modeChooserComboBox, QOverload<int>::of(&QComboBox::activated), this, [=](int index)
    {
        optimiser->setMode(index);
        if(index == 1)
            QMessageBox::warning(this, tr("Several mods option"), tr("You have selected the several mods option. This process may take a very long time, especially if you process BSA. The program may look frozen, but it will work.\nThis process has only been tested on the Mod Organizer mods folder."), QMessageBox::Ok);
    });


    connect(ui->processButton, &QPushButton::pressed, this, [=]()
    {
        optimiser->mainProcess();
    });
}


