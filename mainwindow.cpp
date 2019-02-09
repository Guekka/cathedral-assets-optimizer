#include "mainwindow.hpp"
#include "functions.hpp"

QString modPath;

MainWindow::MainWindow()
{
    bool greyedCreateBsa = false;


    // Window construction

    modpathTextEdit = new QPlainTextEdit(this);
    modpathTextEdit->setFixedHeight(25);

    gridLayout = new QGridLayout(this);
    fileDialog = new QFileDialog(this);
    pathButton = new QPushButton("Open directory", this);
    processButton = new QPushButton("Process directory", this);

    extractBsaCheckbox = new QCheckBox("Extract old BSA", this);
    deleteBsaCheckbox = new QCheckBox("Delete old BSA", this);
    createBsaCheckbox = new QCheckBox("Create new BSA", this);

    textOptCheckbox = new QCheckBox("Optimize textures", this);
    nifOptCheckbox = new QCheckBox("Optimize meshes", this);
    animOptCheckbox = new QCheckBox("Optimize animations", this);

    extractBsaCheckbox->setChecked(true);
    deleteBsaCheckbox->setChecked(true);
    createBsaCheckbox->setChecked(true);

    textOptCheckbox->setChecked(true);
    nifOptCheckbox->setChecked(true);
    animOptCheckbox->setChecked(true);

    log = new QPlainTextEdit(this);
    log->setMinimumHeight(100);
    log->setReadOnly(true);

    this->setLayout(gridLayout);

    gridLayout->addWidget(modpathTextEdit, 0, 0, 1, 2);
    gridLayout->addWidget(pathButton, 0, 2);

    gridLayout->setRowMinimumHeight(1, 15);

    gridLayout->addWidget(processButton, 2,1);

    gridLayout->setRowMinimumHeight(3, 15);

    gridLayout->addWidget(extractBsaCheckbox,4, 0);
    gridLayout->addWidget(deleteBsaCheckbox,4,1);
    gridLayout->addWidget(createBsaCheckbox,4, 2);

    gridLayout->addWidget(textOptCheckbox,5,0);
    gridLayout->addWidget(nifOptCheckbox,5,1);
    gridLayout->addWidget(animOptCheckbox, 5, 2);

    gridLayout->addWidget(log, 6, 0, 3, 0);

    connect(modpathTextEdit, &QPlainTextEdit::textChanged, this, [=](){
        modPath = modpathTextEdit->toPlainText();
    });


    connect(pathButton, &QPushButton::pressed, this, [=](){
        QString dir = QFileDialog::getExistingDirectory(this, "Open Directory", modPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        modpathTextEdit->setPlainText(dir);
        modPath = dir;
    });

    connect(deleteBsaCheckbox, &QCheckBox::pressed, this, [=, &greyedCreateBsa](){
        if(greyedCreateBsa)
        {
            createBsaCheckbox->setDisabled(false);
            greyedCreateBsa=false;
        }else
        {
            createBsaCheckbox->setDisabled(true);
            createBsaCheckbox->setChecked(false);
            greyedCreateBsa=true;
        }
    });

    connect(processButton, &QPushButton::pressed, this, [=]()
    {
        log->clear();
        log->appendPlainText("Beginning...\n");
        log->repaint();

        if(extractBsaCheckbox->isChecked())
        {
            extractBsa(deleteBsaCheckbox->isChecked(), log);
        }
        if(textOptCheckbox->isChecked())
        {
            textOpt(log);
        }
        if(nifOptCheckbox->isChecked())
        {
            nifOpt(log);
        }
        if(createBsaCheckbox->isChecked())
        {
            createBsa(log);
        }

        log->appendHtml("<font color=blue>Completed. Please check the log to check if there have been any errors (in red) </font>\n");
        log->repaint();
    });
}
