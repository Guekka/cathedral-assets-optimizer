/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "OptionsCAO.h"

OptionsCAO::OptionsCAO() {}

OptionsCAO::OptionsCAO(const OptionsCAO& other)
{
    bBsaExtract = other.bBsaExtract;
    bBsaCreate = other.bBsaCreate;
    bBsaDeleteBackup = other.bBsaDeleteBackup;
    bBsaProcessContent = other.bBsaProcessContent;

    bAnimationsOptimization = other.bAnimationsOptimization;

    bDryRun = other.bDryRun;

    bMeshesHeadparts = other.bMeshesHeadparts;
    iMeshesOptimizationLevel = other.iMeshesOptimizationLevel;
    bMeshesResave = other.bMeshesResave;

    iTexturesOptimizationLevel = other.iTexturesOptimizationLevel;
    bTexturesResizeSize = other.bTexturesResizeSize;
    iTexturesTargetWidth = other.iTexturesTargetWidth;
    iTexturesTargetHeight = other.iTexturesTargetHeight;

    bTexturesResizeRatio = other.bTexturesResizeRatio;
    iTexturesTargetWidthRatio = other.iTexturesTargetWidthRatio;
    iTexturesTargetHeightRatio = other.iTexturesTargetHeightRatio;

    iLogLevel = other.iLogLevel;
    mode = other.mode;
    userPath = other.userPath;
}

void OptionsCAO::saveToIni(QSettings *settings)
{
    //General
    settings->setValue("bDryRun", bDryRun);
    settings->setValue("iLogLevel", iLogLevel);
    settings->setValue("mode", mode);
    settings->setValue("userPath", userPath);

    //BSA
    settings->beginGroup("BSA");
    settings->setValue("bBsaExtract", bBsaExtract);
    settings->setValue("bBsaCreate", bBsaCreate);
    settings->setValue("bBsaDeleteBackup", bBsaDeleteBackup);
    settings->setValue("bBsaProcessContent", bBsaProcessContent);
    settings->endGroup();

    //Textures
    settings->beginGroup("Textures");
    settings->setValue("iTexturesOptimizationLevel", iTexturesOptimizationLevel);
    settings->setValue("bTexturesResizeSize", bTexturesResizeSize);
    settings->setValue("iTexturesTargetWidth", iTexturesTargetWidth);
    settings->setValue("iTexturesTargetHeight", iTexturesTargetHeight);

    settings->setValue("bTexturesResizeRatio", bTexturesResizeRatio);
    settings->setValue("iTexturesTargetHeightRatio", iTexturesTargetHeightRatio);
    settings->setValue("iTexturesTargetWidthRatio", iTexturesTargetWidthRatio);
    settings->endGroup();

    //Meshes
    settings->setValue("Meshes/iMeshesOptimizationLevel", iMeshesOptimizationLevel);

    //Meshes advanced
    settings->setValue("Meshes/bMeshesHeadparts", bMeshesHeadparts);
    settings->setValue("Meshes/bMeshesResave", bMeshesResave);

    //Animations
    settings->setValue("Animations/bAnimationsOptimization", bAnimationsOptimization);

    //General
    settings->setValue("bDryRun",bDryRun);
    settings->setValue("iLogLevel", iLogLevel);
    settings->setValue("mode", mode);
    settings->setValue("userPath", userPath);
}

void OptionsCAO::readFromIni(QSettings *settings)
{
    //General
    bDryRun = settings->value("bDryRun").toBool();
    iLogLevel = settings->value("iLogLevel").toInt();
    mode = settings->value("mode").value<OptimizationMode>();
    userPath = settings->value("userPath").toString();

    //BSA
    settings->beginGroup("BSA");
    bBsaExtract = settings->value("bBsaExtract").toBool();
    bBsaCreate = settings->value("bBsaCreate").toBool();
    bBsaDeleteBackup = settings->value("bBsaDeleteBackup").toBool();
    bBsaProcessContent = settings->value("bBsaProcessContent").toBool();
    settings->endGroup();

    //Textures
    settings->beginGroup("Textures");
    iTexturesOptimizationLevel = settings->value("iTexturesOptimizationLevel").toInt();
    bTexturesResizeSize = settings->value("bTexturesResizeSize").toBool();
    iTexturesTargetWidth = settings->value("iTexturesTargetWidth").toUInt();
    iTexturesTargetHeight = settings->value("iTexturesTargetHeight").toUInt();

    bTexturesResizeRatio = settings->value("bTexturesResizeRatio").toBool();
    iTexturesTargetWidthRatio = settings->value("iTexturesTargetWidthRatio").toUInt();
    iTexturesTargetHeightRatio = settings->value("iTexturesTargetHeightRatio").toUInt();
    settings->endGroup();

    //Meshes
    iMeshesOptimizationLevel = settings->value("Meshes/iMeshesOptimizationLevel").toInt();

    //Meshes advanced
    bMeshesHeadparts = settings->value("Meshes/bMeshesHeadparts").toBool();
    bMeshesResave = settings->value("Meshes/bMeshesResave").toBool();

    //Animations
    bAnimationsOptimization = settings->value("Animations/bAnimationsOptimization").toBool();
}

#ifdef GUI
void OptionsCAO::saveToUi(Ui::MainWindow *ui)
{
    //BSA
    ui->bsaExtractCheckBox->setChecked(bBsaExtract);
    ui->bsaCreateCheckbox->setChecked(bBsaCreate);
    ui->bsaDeleteBackupsCheckbox->setChecked(bBsaDeleteBackup);
    ui->bsaProcessContentCheckBox->setChecked(bBsaProcessContent);

    //Textures

    ui->texturesGroupBox->setChecked(true);
    switch (iTexturesOptimizationLevel)
    {
    case 0: ui->texturesGroupBox->setChecked(false); break;
    case 1: ui->texturesNecessaryOptimizationRadioButton->setChecked(true);  break;
    case 2: ui->texturesMediumOptimizationRadioButton->setChecked(true); break;
    case 3: ui->texturesFullOptimizationRadioButton->setChecked(true); break;
    }

    ui->texturesResizingGroupBox->setChecked(bTexturesResizeSize || bTexturesResizeRatio);

    ui->texturesResizingBySizeRadioButton->setChecked(bTexturesResizeSize);
    ui->texturesResizingBySizeWidth->setValue(static_cast<int>(iTexturesTargetWidth));
    ui->texturesResizingBySizeHeight->setValue(static_cast<int>(iTexturesTargetHeight));

    ui->texturesResizingByRatioRadioButton->setChecked(bTexturesResizeRatio);
    ui->texturesResizingByRatioWidth->setValue(static_cast<int>(iTexturesTargetWidthRatio));
    ui->texturesResizingByRatioHeight->setValue(static_cast<int>(iTexturesTargetHeightRatio));

    //Meshes

    ui->meshesGroupBox->setChecked(true);
    switch(iMeshesOptimizationLevel)
    {
    case 0: ui->meshesGroupBox->setChecked(false); break;
    case 1: ui->meshesNecessaryOptimizationRadioButton->setChecked(true);  break;
    case 2: ui->meshesMediumOptimizationRadioButton->setChecked(true); break;
    case 3: ui->meshesFullOptimizationRadioButton->setChecked(true); break;
    }

    ui->meshesResaveCheckBox->setChecked(bMeshesResave);
    ui->meshesHeadpartsCheckBox->setChecked(bMeshesHeadparts);

    //Animations
    ui->animationsGroupBox->setChecked(bAnimationsOptimization);

    //General and GUI
    ui->dryRunCheckBox->setChecked(bDryRun);
    ui->modeChooserComboBox->setCurrentIndex(mode);
    ui->userPathTextEdit->setText(userPath);

    //Log level
    ui->actionLogVerbosityInfo->setChecked(false);
    ui->actionLogVerbosityNote->setChecked(false);
    ui->actionLogVerbosityTrace->setChecked(false);

    switch (iLogLevel)
    {
    case 6: ui->actionLogVerbosityTrace->setChecked(true); break;
    case 4: ui->actionLogVerbosityNote->setChecked(true); break;
    case 3: ui->actionLogVerbosityInfo->setChecked(true); break;
    }
}

void OptionsCAO::readFromUi(Ui::MainWindow *ui)
{
    //BSA
    bBsaExtract = ui->bsaExtractCheckBox->isChecked();
    bBsaCreate = ui->bsaCreateCheckbox->isChecked();
    bBsaDeleteBackup =ui->bsaDeleteBackupsCheckbox->isChecked();
    bBsaProcessContent = ui->bsaProcessContentCheckBox->isChecked();

    //Textures
    if(ui->texturesNecessaryOptimizationRadioButton->isChecked())
        iTexturesOptimizationLevel = 1;
    else if(ui->texturesMediumOptimizationRadioButton->isChecked())
        iTexturesOptimizationLevel = 2;
    else if(ui->texturesFullOptimizationRadioButton->isChecked())
        iTexturesOptimizationLevel = 3;
    if(!ui->texturesGroupBox->isChecked())
        iTexturesOptimizationLevel = 0;

    bTexturesResizeSize = ui->texturesResizingBySizeRadioButton->isChecked();
    iTexturesTargetWidth = static_cast<size_t>(ui->texturesResizingBySizeWidth->value());
    iTexturesTargetHeight = static_cast<size_t>(ui->texturesResizingBySizeHeight->value());

    bTexturesResizeRatio = ui->texturesResizingByRatioRadioButton->isChecked();
    iTexturesTargetWidthRatio = static_cast<size_t>(ui->texturesResizingByRatioWidth->value());
    iTexturesTargetHeightRatio = static_cast<size_t>(ui->texturesResizingByRatioHeight->value());

    //Meshes base
    if(ui->meshesNecessaryOptimizationRadioButton->isChecked())
        iMeshesOptimizationLevel = 1;
    else if(ui->meshesMediumOptimizationRadioButton->isChecked())
        iMeshesOptimizationLevel = 2;
    else if(ui->meshesFullOptimizationRadioButton->isChecked())
        iMeshesOptimizationLevel = 3;
    if(!ui->meshesGroupBox->isChecked())
        iMeshesOptimizationLevel = 0;

    //Meshes advanced
    bMeshesHeadparts = ui->meshesHeadpartsCheckBox->isChecked();
    bMeshesResave = ui->meshesResaveCheckBox->isChecked();

    //Animations
    bAnimationsOptimization = ui->animationsNecessaryOptimizationCheckBox->isChecked();

    //General
    bDryRun = ui->dryRunCheckBox->isChecked();
    userPath = QDir::cleanPath(ui->userPathTextEdit->text());
    mode = ui->modeChooserComboBox->currentData().value<OptimizationMode>();

    if(ui->actionLogVerbosityNote->isChecked())
        iLogLevel = 4;
    else if(ui->actionLogVerbosityTrace->isChecked())
        iLogLevel = 6;
    else
        iLogLevel = 3;
}
#endif

void OptionsCAO::parseArguments(const QStringList &args)
{
    QCommandLineParser parser;

    parser.addHelpOption();
    parser.addPositionalArgument("folder", "The folder to process, surrounded with quotes.");
    parser.addPositionalArgument("mode", "Either om (one mod) or sm (several mods)");
    parser.addPositionalArgument("game", "Currently, only 'SSE', 'TES5', 'FO4' and 'Custom' are supported");

    parser.addOptions({
                          {"dr", "Enables dry run"},
                          {"l", "Log level: from 1 (maximum) to 6", "value", "1"},
                          {"m", "Mesh processing level: 0 (default) to disable optimization, 1 for necessary optimization, "
                           "2 for medium optimization, 3 for full optimization.", "value", "0"},

                          {"t", "Texture processing level: 0 (default) to disable optimization, "
                           "1 for necessary optimization, 2 for full optimization.", "value", "0"},

                          {"a", "Enables animations processing"},
                          {"mh", "Enables headparts detection and processing"},
                          {"mr", "Enables meshes resaving"},
                          {"be", "Enables BSA extraction."},
                          {"bc", "Enables BSA creation."},
                          {"bd", "Enables deletion of BSA backups."},
                          {"bo", "NOT WORKING. Enables BSA optimization. The files inside the "
                           "BSA will be extracted to memory and processed according to the provided settings "},
                      });

    parser.process(args);

    QString path = QDir::cleanPath(parser.positionalArguments().at(0));
    userPath = path;

    QString readMode = parser.positionalArguments().at(1);
    if(readMode == "om")
        mode = singleMod;
    else if(readMode == "sm")
        mode = severalMods;

    QString readGame = parser.positionalArguments().at(2);
    CAO_SET_CURRENT_GAME(readGame)

    bDryRun = parser.isSet("dr");
    iLogLevel = parser.value("l").toInt();

    iMeshesOptimizationLevel = parser.value("m").toInt();
    bMeshesHeadparts = parser.isSet("mh");
    bMeshesResave = parser.isSet("mr");

    iTexturesOptimizationLevel = parser.value("t").toInt();

    bAnimationsOptimization = parser.isSet("a");

    bBsaExtract = parser.isSet("be");
    bBsaCreate = parser.isSet("bc");
    bBsaDeleteBackup = parser.isSet("bd");
    bBsaProcessContent = parser.isSet("bo");
}

QString OptionsCAO::isValid()
{
    if(!QDir(userPath).exists() || userPath.size() < 5)
        return("This path does not exist or is shorter than 5 characters. Path:'" + userPath + "'");

    if (mode != OptionsCAO::singleMod && mode != OptionsCAO::severalMods)
        return("This mode does not exist.");

    if(iLogLevel <= 0 || iLogLevel > 6)
        return("This log level does not exist. Log level: " + QString::number(iLogLevel));

    if(iMeshesOptimizationLevel < 0 || iMeshesOptimizationLevel > 3)
        return("This meshes optimization level does not exist. Level: " + QString::number(iMeshesOptimizationLevel));

    if(iTexturesOptimizationLevel < 0 || iTexturesOptimizationLevel > 3)
        return("This textures optimization level does not exist. Level: " + QString::number(iTexturesOptimizationLevel));

    return QString();
}
