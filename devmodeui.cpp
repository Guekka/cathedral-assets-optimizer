#include "devmodeui.h"
#include "ui_devmodeui.h"
#include "Optimiser.hpp"


devModeUI::devModeUI( Optimiser *optimiser) :
    ui(new Ui::devModeUI)
{
    ui->devModeLog->appendPlainText("test");

    ui->setupUi(this);

    connect(ui->CreateBSA, &QPushButton::clicked, this, [=]()
    {
        optimiser->createBsa();
    });

    connect(ui->ExtractBSA, &QPushButton::clicked, this, [=]()
    {
        optimiser->extractBsa();
    });

    connect(ui->FindEspName, &QPushButton::clicked, this, [=]()
    {
        optimiser->findEspName();
    });

    connect(ui->RenameBSA, &QPushButton::clicked, this, [=]()
    {
        optimiser->renameBsa();
    });

    connect(ui->pushButton, &QPushButton::clicked, this, [=]()
    {
        optimiser->printSettings();
    });

    connect(ui->pushButton_2, &QPushButton::clicked, this, [=]()
    {
        ui->devModeLog->clear();
    });

    connect(ui->Setup, &QPushButton::clicked, this, [=]()
    {
        optimiser->setup();
    });
}

devModeUI::~devModeUI()
{
    delete ui;
}
