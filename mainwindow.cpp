#include "mainwindow.h"
#include "functions.h"

QString modPath;

MainWindow::MainWindow()
{

    bool extractBsa(true), deleteBsa(false), optimizeTextures(true), optimizeMeshes(true);


    // Window construction

    textEdit = new QPlainTextEdit(this);
    textEdit->setFixedHeight(25);

    log = new QPlainTextEdit(this);
    log->setFixedHeight(100);
    log->setReadOnly(true);

    gridLayout = new QGridLayout(this);
    fileDialog = new QFileDialog(this);
    pathButton = new QPushButton("Open directory", this);
    processButton = new QPushButton("Process directory", this);

    extractBsaCheckbox = new QCheckBox("Extract BSA", this);
    deleteBsaCheckbox = new QCheckBox("Delete BSA", this);
    textOptCheckbox = new QCheckBox("Optimize textures", this);
    nifOptCheckbox = new QCheckBox("Optimize meshes", this);

    extractBsaCheckbox->setChecked(true);
    deleteBsaCheckbox->setChecked(false);
    textOptCheckbox->setChecked(true);
    nifOptCheckbox->setChecked(true);

    this->setLayout(gridLayout);
    gridLayout->addWidget(textEdit, 0, 0);
    gridLayout->addWidget(pathButton, 0, 1);
    gridLayout->addWidget(processButton, 2,0, 2, 2, Qt::AlignCenter );
    gridLayout->addWidget(extractBsaCheckbox,4, 0);
    gridLayout->addWidget(deleteBsaCheckbox,4,1);
    gridLayout->addWidget(textOptCheckbox,5,0);
    gridLayout->addWidget(nifOptCheckbox,5,1);
    gridLayout->addWidget(log, 6, 0, 2, 0);

    connect(textEdit, &QPlainTextEdit::textChanged, this, [=](){
        modPath = textEdit->toPlainText();
    });

    connect(nifOptCheckbox, &QCheckBox::stateChanged, this ,[=, &optimizeMeshes](){
        optimizeMeshes = nifOptCheckbox->isChecked();

    });

    connect(extractBsaCheckbox, &QCheckBox::stateChanged, this, [=, &extractBsa](){
        extractBsa = extractBsaCheckbox->isChecked();

    });

    connect(deleteBsaCheckbox, &QCheckBox::stateChanged, this,[=, &deleteBsa](){
        deleteBsa = deleteBsaCheckbox->isChecked();

    });

    connect(textOptCheckbox, &QCheckBox::stateChanged, this, [=, &optimizeTextures](){
        optimizeTextures = textOptCheckbox->isChecked();
    });





    connect(pathButton, &QPushButton::pressed, this, [=](){
        QString dir = QFileDialog::getExistingDirectory(this, "Open Directory", modPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        textEdit->setPlainText(dir);
        modPath = dir;
    });


    connect(processButton, &QPushButton::pressed, this, [=]()
    {
        mainFunc(extractBsa, deleteBsa, optimizeMeshes, optimizeMeshes, log);
    });

}
