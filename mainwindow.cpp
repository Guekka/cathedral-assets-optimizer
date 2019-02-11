#include "mainwindow.hpp"
#include "optimiser.hpp"



MainWindow::MainWindow()
{
    bool greyedCreateBsa = false;

    // Window construction

    modpathTextEdit = new QPlainTextEdit(this);
    modpathTextEdit->setFixedHeight(25);

    gridLayout = new QGridLayout(this);
    fileDialog = new QFileDialog(this);
    pathButton = new QPushButton(tr("Open directory"), this);
    processButton = new QPushButton(tr("Process directory"), this);

    extractBsaCheckbox = new QCheckBox(tr("Extract old BSA"), this);
    deleteBsaCheckbox = new QCheckBox(tr("Delete old BSA"), this);
    createBsaCheckbox = new QCheckBox("Create new BSA", this);

    textOptCheckbox = new QCheckBox(tr("Optimise textures"), this);
    nifOptCheckbox = new QCheckBox(tr("Optimise meshes"), this);
    animOptCheckbox = new QCheckBox(tr("Optimise animations"), this);

    extractBsaCheckbox->setChecked(true);
    deleteBsaCheckbox->setChecked(true);
    createBsaCheckbox->setChecked(true);

    textOptCheckbox->setChecked(true);
    nifOptCheckbox->setChecked(true);
    animOptCheckbox->setChecked(true);

    mw_log = new QPlainTextEdit(this);
    mw_log->setMinimumHeight(100);
    mw_log->setReadOnly(true);

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

    gridLayout->addWidget(mw_log, 6, 0, 3, 0);


    optimiser = new Optimiser("C:/", mw_log);


    connect(modpathTextEdit, &QPlainTextEdit::textChanged, this, [=](){
        optimiser->setmodPath(modpathTextEdit->toPlainText());
    });


    connect(pathButton, &QPushButton::pressed, this, [=](){
        QString dir = QFileDialog::getExistingDirectory(this, "Open Directory", optimiser->getmodPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        modpathTextEdit->setPlainText(dir);
        optimiser->setmodPath(dir);
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
        optimiser->setExtractBsaBool(extractBsaCheckbox->isChecked());
        optimiser->setDeleteBsaBool(deleteBsaCheckbox->isChecked());
        optimiser->setTextOptBool(textOptCheckbox->isChecked());
        optimiser->setNifOptBool(nifOptCheckbox->isChecked());
        optimiser->setAnimOptBool(animOptCheckbox->isChecked());
        optimiser->setCreateBsaBool(createBsaCheckbox->isChecked());

        optimiser->mainProcess();
    });
}
