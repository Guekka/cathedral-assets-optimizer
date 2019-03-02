#include "devmodeui.h"
#include "ui_devmodeui.h"
#include "Optimiser.hpp"


devModeUI::devModeUI(QWidget *parent) :
    QWidget (parent),
    ui(new Ui::devModeUI)
{
    ui->setupUi(this);

    optimiser = new Optimiser(ui->log, ui->log, ui->progressBar);
    optimiser->loadSettings();

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
        ui->log->clear();
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
