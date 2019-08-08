/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "OptionsCAO.h"

OptionsCAO::OptionsCAO()
    : mode(SingleMod)
{
}

OptionsCAO::OptionsCAO(const OptionsCAO &other)
    : bBsaExtract(other.bBsaExtract)
    , bBsaCreate(other.bBsaCreate)
    , bBsaDeleteBackup(other.bBsaDeleteBackup)
    , bBsaProcessContent(other.bBsaProcessContent)
    ,

    bAnimationsOptimization(other.bAnimationsOptimization)
    ,

    bDryRun(other.bDryRun)
    ,

    iMeshesOptimizationLevel(other.iMeshesOptimizationLevel)
    , bMeshesHeadparts(other.bMeshesHeadparts)
    , bMeshesResave(other.bMeshesResave)
    ,

    bTexturesNecessary(other.bTexturesNecessary)
    , bTexturesCompress(other.bTexturesCompress)
    , bTexturesMipmaps(other.bTexturesMipmaps)
    ,

    bTexturesResizeSize(other.bTexturesResizeSize)
    , iTexturesTargetHeight(other.iTexturesTargetHeight)
    , iTexturesTargetWidth(other.iTexturesTargetWidth)
    ,

    bTexturesResizeRatio(other.bTexturesResizeRatio)
    , iTexturesTargetWidthRatio(other.iTexturesTargetWidthRatio)
    , iTexturesTargetHeightRatio(other.iTexturesTargetHeightRatio)
    , bDebugLog(other.bDebugLog)
    , mode(other.mode)
    , userPath(other.userPath)
{
}

void OptionsCAO::saveToIni(QSettings *settings) const
{
    //General
    settings->setValue("bDryRun", bDryRun);
    settings->setValue("bDebugLog", bDebugLog);
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
    settings->setValue("bTexturesNecessary", bTexturesNecessary);
    settings->setValue("bTexturesCompress", bTexturesCompress);
    settings->setValue("bTexturesMipmaps", bTexturesMipmaps);

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
}

void OptionsCAO::readFromIni(QSettings *settings)
{
    //General
    bDryRun = settings->value("bDryRun").toBool();
    bDebugLog = settings->value("bDebugLog").toBool();
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
    bTexturesNecessary = settings->value("bTexturesNecessary").toBool();
    bTexturesCompress = settings->value("bTexturesCompress").toBool();
    bTexturesMipmaps = settings->value("bTexturesMipmaps").toBool();

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

    //Textures
    const bool texturesOpt = bTexturesMipmaps || bTexturesCompress || bTexturesNecessary;
    if (!texturesOpt)
        ui->texturesGroupBox->setChecked(false);
    else
    {
        ui->texturesGroupBox->setChecked(true);
        ui->texturesNecessaryOptimizationCheckBox->setChecked(bTexturesNecessary);
        ui->texturesCompressCheckBox->setChecked(bTexturesCompress);
        ui->texturesMipmapCheckBox->setChecked(bTexturesMipmaps);
    }

    //Textures resizing
    ui->texturesResizingGroupBox->setChecked(bTexturesResizeSize || bTexturesResizeRatio);

    ui->texturesResizingBySizeRadioButton->setChecked(bTexturesResizeSize);
    ui->texturesResizingBySizeWidth->setValue(static_cast<int>(iTexturesTargetWidth));
    ui->texturesResizingBySizeHeight->setValue(static_cast<int>(iTexturesTargetHeight));

    ui->texturesResizingByRatioRadioButton->setChecked(bTexturesResizeRatio);
    ui->texturesResizingByRatioWidth->setValue(static_cast<int>(iTexturesTargetWidthRatio));
    ui->texturesResizingByRatioHeight->setValue(static_cast<int>(iTexturesTargetHeightRatio));

    //Meshes

    ui->meshesGroupBox->setChecked(true);
    switch (iMeshesOptimizationLevel)
    {
    case 0: ui->meshesGroupBox->setChecked(false); break;
    case 1: ui->meshesNecessaryOptimizationRadioButton->setChecked(true); break;
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
    ui->actionEnable_debug_log->setChecked(bDebugLog);
}

void OptionsCAO::readFromUi(Ui::MainWindow *ui)
{
    //BSA
    const bool bsaEnabled = ui->bsaTab->isEnabled();
    bBsaExtract = bsaEnabled && ui->bsaExtractCheckBox->isChecked();
    bBsaCreate = bsaEnabled && ui->bsaCreateCheckbox->isChecked();
    bBsaDeleteBackup = bsaEnabled && ui->bsaDeleteBackupsCheckbox->isChecked();

    //Textures
    const bool texturesEnabled = ui->texturesGroupBox->isChecked() && ui->texturesTab->isEnabled();
    bTexturesNecessary = texturesEnabled && ui->texturesNecessaryOptimizationCheckBox->isChecked();
    bTexturesMipmaps = texturesEnabled && ui->texturesMipmapCheckBox->isChecked();
    bTexturesCompress = texturesEnabled && ui->texturesCompressCheckBox->isChecked();

    //Textures resizing
    const bool texturesResizing = ui->texturesResizingGroupBox->isChecked() && ui->texturesTab->isEnabled();
    bTexturesResizeSize = ui->texturesResizingBySizeRadioButton->isChecked() && texturesResizing;
    iTexturesTargetWidth = static_cast<size_t>(ui->texturesResizingBySizeWidth->value());
    iTexturesTargetHeight = static_cast<size_t>(ui->texturesResizingBySizeHeight->value());

    bTexturesResizeRatio = ui->texturesResizingByRatioRadioButton->isChecked() && texturesResizing;
    iTexturesTargetWidthRatio = static_cast<size_t>(ui->texturesResizingByRatioWidth->value());
    iTexturesTargetHeightRatio = static_cast<size_t>(ui->texturesResizingByRatioHeight->value());

    //Meshes base
    const bool meshesEnabled = ui->meshesTab->isEnabled();
    if (ui->meshesNecessaryOptimizationRadioButton->isChecked())
        iMeshesOptimizationLevel = 1;
    else if (ui->meshesMediumOptimizationRadioButton->isChecked())
        iMeshesOptimizationLevel = 2;
    else if (ui->meshesFullOptimizationRadioButton->isChecked())
        iMeshesOptimizationLevel = 3;
    if (!ui->meshesGroupBox->isChecked() || !meshesEnabled)
        iMeshesOptimizationLevel = 0;

    //Meshes advanced
    bMeshesHeadparts = meshesEnabled && ui->meshesHeadpartsCheckBox->isChecked();
    bMeshesResave = meshesEnabled && ui->meshesResaveCheckBox->isChecked();

    //Animations
    bAnimationsOptimization = ui->AnimationsTab->isEnabled()
                              && ui->animationsNecessaryOptimizationCheckBox->isChecked();

    //General
    bDryRun = ui->dryRunCheckBox->isChecked();
    userPath = QDir::cleanPath(ui->userPathTextEdit->text());
    mode = ui->modeChooserComboBox->currentData().value<OptimizationMode>();

    bDebugLog = ui->actionEnable_debug_log->isChecked();
}
#endif

void OptionsCAO::parseArguments(const QStringList &args)
{
    //TODO update before release
    QCommandLineParser parser;

    parser.addHelpOption();
    parser.addPositionalArgument("folder", "The folder to process, surrounded with quotes.");
    parser.addPositionalArgument("mode", "Either om (one mod) or sm (several mods)");
    parser.addPositionalArgument("game", "Currently, only 'SSE', 'TES5', 'FO4' and 'Custom' are supported");

    parser.addOptions({
        {"dr", "Enables dry run"},
        {"l", "Enables debug log"},
        {"m",
         "Mesh processing level: 0 (default) to disable optimization, 1 for necessary optimization, "
         "2 for medium optimization, 3 for full optimization.",
         "value",
         "0"},

        {"t0", "Enables textures necessary optimization"},
        {"t1", "Enables textures compression"},
        {"t2", "Enables textures mipmaps generation"},

        {"a", "Enables animations processing"},
        {"mh", "Enables headparts detection and processing"},
        {"mr", "Enables meshes resaving"},
        {"be", "Enables BSA extraction."},
        {"bc", "Enables BSA creation."},
        {"bd", "Enables deletion of BSA backups."},
        {"bo",
         "NOT WORKING. Enables BSA optimization. The files inside the "
         "BSA will be extracted to memory and processed according to the provided settings "},
    });

    parser.process(args);

    const QString path = QDir::cleanPath(parser.positionalArguments().at(0));
    userPath = path;

    const QString readMode = parser.positionalArguments().at(1);
    if (readMode == "om")
        mode = SingleMod;
    else if (readMode == "sm")
        mode = SeveralMods;

    const QString readGame = parser.positionalArguments().at(2);
    Profiles::setCurrentProfile(readGame);

    bDryRun = parser.isSet("dr");
    bDebugLog = parser.isSet("l");

    iMeshesOptimizationLevel = parser.value("m").toInt();
    bMeshesHeadparts = parser.isSet("mh");
    bMeshesResave = parser.isSet("mr");

    bTexturesNecessary = parser.isSet("t0");
    bTexturesCompress = parser.isSet("t1");
    bTexturesMipmaps = parser.isSet("t2");

    bAnimationsOptimization = parser.isSet("a");

    bBsaExtract = parser.isSet("be");
    bBsaCreate = parser.isSet("bc");
    bBsaDeleteBackup = parser.isSet("bd");
    bBsaProcessContent = parser.isSet("bo");
}

QString OptionsCAO::isValid() const
{
    if (!QDir(userPath).exists() || userPath.size() < 5)
        return (tr("This path does not exist or is shorter than 5 characters. Path:") + " '" + userPath + "'");

    if (mode != SingleMod && mode != SeveralMods)
        return tr("This mode does not exist.");

    if (iMeshesOptimizationLevel < 0 || iMeshesOptimizationLevel > 3)
        return (tr("This meshes optimization level does not exist. Level: ")
                + QString::number(iMeshesOptimizationLevel));

    return QString();
}
