#include "mainwindow.hpp"
#include "Optimiser.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow() : ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    optimiser = new Optimiser(ui->mw_log, ui->progressBar);
    optimiser->loadSettings();

    ui->bsaOptCheckbox->setChecked(optimiser->options.bsaOpt);


    ui->extractBsaCheckbox->setChecked(optimiser->options.extractBsa);
    ui->renameBsaCheckbox->setChecked(optimiser->options.renameBsa);
    ui->createBsaCheckbox->setChecked(optimiser->options.createBsa);

    if(!ui->renameBsaCheckbox->isChecked())
    {
        ui->createBsaCheckbox->setEnabled(false);
        ui->createBsaCheckbox->setChecked(false);
    }

    ui->textOptCheckbox->setChecked(optimiser->options.textOpt);
    ui->meshOptCheckbox->setChecked(optimiser->options.nifOpt);
    ui->animOptCheckbox->setChecked(optimiser->options.animOpt);


    connect(ui->userPathTextEdit, &QPlainTextEdit::textChanged, this, [=](){
        optimiser->options.userPath = ui->userPathTextEdit->toPlainText();
    });


    connect(ui->userPathButton, &QPushButton::pressed, this, [=](){
        QString dir = QFileDialog::getExistingDirectory(this, "Open Directory", optimiser->options.userPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        ui->userPathTextEdit->setPlainText(dir);
        optimiser->options.userPath = dir;
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
        optimiser->options.extractBsa = !ui->extractBsaCheckbox->isChecked();
    });


    connect(ui->renameBsaCheckbox, &QCheckBox::pressed, this, [=]()
    {
        optimiser->options.renameBsa = !ui->renameBsaCheckbox->isChecked();
    });

    connect(ui->bsaOptCheckbox, &QCheckBox::pressed, this, [=]()
    {
        optimiser->options.bsaOpt = !ui->bsaOptCheckbox->isChecked();

        optimiser->options.createBsa = !ui->bsaOptCheckbox->isChecked();
        optimiser->options.renameBsa = !ui->bsaOptCheckbox->isChecked();
        optimiser->options.extractBsa = !ui->bsaOptCheckbox->isChecked();

        ui->extractBsaCheckbox->setChecked(optimiser->options.extractBsa);
        ui->renameBsaCheckbox->setChecked(optimiser->options.renameBsa);
        ui->createBsaCheckbox->setChecked(optimiser->options.createBsa);
    });

    connect(ui->textOptCheckbox, &QCheckBox::pressed, this, [=]()
    {
        optimiser->options.textOpt = !ui->textOptCheckbox->isChecked();

        optimiser->options.bc7Conv = !ui->textOptCheckbox->isChecked();
        optimiser->options.tgaConv = !ui->textOptCheckbox->isChecked();
        optimiser->options.nifscanTextures = !ui->textOptCheckbox->isChecked();

        ui->bc7ConvCheckbox->setChecked(optimiser->options.bc7Conv);
        ui->tgaConvCheckbox->setChecked(optimiser->options.tgaConv);
        ui->nifscanTexturesCheckbox->setChecked(optimiser->options.nifscanTextures);
    });

    connect(ui->meshOptCheckbox, &QCheckBox::pressed, this, [=]()
    {
        optimiser->options.nifOpt = !ui->meshOptCheckbox->isChecked();
    });

    connect(ui->animOptCheckbox, &QCheckBox::pressed, this, [=]()
    {
        optimiser->options.animOpt = !ui->animOptCheckbox->isChecked();
    });

    connect(ui->createBsaCheckbox, &QCheckBox::pressed, this, [=]()
    {
        optimiser->options.createBsa = !ui->createBsaCheckbox->isChecked();
    });

    connect(ui->modeChooserComboBox, QOverload<int>::of(&QComboBox::activated), this, [=](int index)
    {
        optimiser->options.mode = index;
        if(index == 1)
            QMessageBox::warning(this, tr("Several mods option"), tr("You have selected the several mods option. This process may take a very long time, especially if you process BSA. The program may look frozen, but it will work.\nThis process has only been tested on the Mod Organizer mods folder."), QMessageBox::Ok);
    });


    connect(ui->processButton, &QPushButton::pressed, this, [=]()
    {
        optimiser->mainProcess();
    });
}


