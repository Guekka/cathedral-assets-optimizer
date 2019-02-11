#include "mainwindow.hpp"
#include "Optimiser.hpp"



MainWindow::MainWindow()
{
    //Constructing log before the rest to give a pointer to Optimiser

    mw_log = new QPlainTextEdit(this);
    mw_log->setMinimumHeight(100);
    mw_log->setReadOnly(true);

    optimiser = new Optimiser("C:/", mw_log);
    optimiser->loadSettings();

    // Window construction

    modpathTextEdit = new QPlainTextEdit(this);
    modpathTextEdit->setFixedHeight(25);
    modpathTextEdit->appendPlainText(optimiser->getUserPath());

    gridLayout = new QGridLayout(this);
    fileDialog = new QFileDialog(this);
    pathButton = new QPushButton(tr("Open directory"), this);
    processButton = new QPushButton(tr("Process directory"), this);


    dropDown = new QComboBox(this);
    dropDown->addItem("One mod");
    dropDown->addItem("Several mods");

    extractBsaCheckbox = new QCheckBox(tr("Extract old BSA"), this);
    renameBsaCheckbox = new QCheckBox(tr("Rename old BSA"), this);
    createBsaCheckbox = new QCheckBox("Create new BSA", this);

    textOptCheckbox = new QCheckBox(tr("Optimise textures"), this);
    nifOptCheckbox = new QCheckBox(tr("Optimise meshes"), this);
    animOptCheckbox = new QCheckBox(tr("Optimise animations"), this);


    extractBsaCheckbox->setChecked(optimiser->getExtractBsaBool());
    renameBsaCheckbox->setChecked(optimiser->getrenameBsaBool());
    createBsaCheckbox->setChecked(optimiser->getCreateBsaBool());

    if(!renameBsaCheckbox->isChecked())
    {
        createBsaCheckbox->setEnabled(false);
        createBsaCheckbox->setChecked(false);
    }

    textOptCheckbox->setChecked(optimiser->getTextOptBool());
    nifOptCheckbox->setChecked(optimiser->getNifOptBool());
    animOptCheckbox->setChecked(optimiser->getAnimOptBool());

    this->setLayout(gridLayout);

    gridLayout->addWidget(modpathTextEdit, 0, 0, 2, 0);

    gridLayout->setRowMinimumHeight(1, 15);

    gridLayout->addWidget(pathButton, 2, 0);
    gridLayout->addWidget(processButton, 2,1);
    gridLayout->addWidget(dropDown, 2,2);

    gridLayout->setRowMinimumHeight(3, 15);

    gridLayout->addWidget(extractBsaCheckbox,4, 0);
    gridLayout->addWidget(renameBsaCheckbox,4,1);
    gridLayout->addWidget(createBsaCheckbox,4, 2);

    gridLayout->addWidget(textOptCheckbox,5,0);
    gridLayout->addWidget(nifOptCheckbox,5,1);
    gridLayout->addWidget(animOptCheckbox, 5, 2);

    gridLayout->addWidget(mw_log, 6, 0, 2, 0);




    connect(modpathTextEdit, &QPlainTextEdit::textChanged, this, [=](){
        optimiser->setUserPath(modpathTextEdit->toPlainText());
    });


    connect(pathButton, &QPushButton::pressed, this, [=](){
        QString dir = QFileDialog::getExistingDirectory(this, "Open Directory", optimiser->getUserPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        modpathTextEdit->setPlainText(dir);
        optimiser->setUserPath(dir);
    });

    connect(renameBsaCheckbox, &QCheckBox::pressed, this, [=](){
        if(renameBsaCheckbox->isChecked())
        {
            createBsaCheckbox->setDisabled(true);
            createBsaCheckbox->setChecked(false);
        }else
        {
            createBsaCheckbox->setDisabled(false);
        }
    });

    connect(extractBsaCheckbox, &QCheckBox::pressed, this, [=]()
    {
        optimiser->setExtractBsaBool(extractBsaCheckbox->isChecked());
    });


    connect(renameBsaCheckbox, &QCheckBox::pressed, this, [=]()
    {
        optimiser->setRenameBsaBool(renameBsaCheckbox->isChecked());
    });

    connect(textOptCheckbox, &QCheckBox::pressed, this, [=]()
    {
        optimiser->setTextOptBool(textOptCheckbox->isChecked());
    });

    connect(nifOptCheckbox, &QCheckBox::pressed, this, [=]()
    {
        optimiser->setNifOptBool(nifOptCheckbox->isChecked());
    });

    connect(animOptCheckbox, &QCheckBox::pressed, this, [=]()
    {
        optimiser->setAnimOptBool(animOptCheckbox->isChecked());
    });

    connect(createBsaCheckbox, &QCheckBox::pressed, this, [=]()
    {
        optimiser->setCreateBsaBool(createBsaCheckbox->isChecked());
    });

    connect(dropDown, QOverload<int>::of(&QComboBox::activated), this, [=](int index)
    {
        optimiser->setMode(index);
    });


    connect(processButton, &QPushButton::pressed, this, [=]()
    {
        optimiser->mainProcess();
    });
}

