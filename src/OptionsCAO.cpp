/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "OptionsCAO.hpp"
namespace CAO {
OptionsCAO::OptionsCAO()
    : mutex(new QMutex)
{
}

void OptionsCAO::saveToJSON(const QString &filepath) const
{
    QMutexLocker lock(mutex);
    std::fstream stream(std::filesystem::u8path(filepath.toStdString()));

    stream << _json;
}

void OptionsCAO::readFromJSON(const QString &filepath)
{
    QMutexLocker lock(mutex);

    std::fstream stream(std::filesystem::u8path(filepath.toStdString()));
    stream >> _json;
}

template<class T>
T OptionsCAO::getMandatoryValue(const QString &key) const
{
    return splitKeySafe(key).get<T>();
}

template<>
QString OptionsCAO::getMandatoryValue(const QString &key) const
{
    auto value = splitKeySafe(key);
    return QString::fromStdString(value.get<std::string>());
}

template<class T>
T OptionsCAO::getMandatoryValue(const StandardKeys &key) const
{
    return enumToKey(key).get<T>();
}

template<>
QString OptionsCAO::getMandatoryValue(const StandardKeys &key) const
{
    auto value = enumToKey(key);
    return QString::fromStdString(value.get<std::string>());
}

template<class T>
T OptionsCAO::getOptionalValue(const QString &key) const
{
    return splitKey(key).get<T>();
}

template<>
QString OptionsCAO::getOptionalValue(const QString &key) const
{
    auto value = splitKey(key);
    return QString::fromStdString(value.get<std::string>());
}

template<class T>
void OptionsCAO::setValue(const QString &key, const T &value)
{
    splitKey(key) = value;
}

template<>
void OptionsCAO::setValue(const QString &key, const QString &value)
{
    splitKey(key) = value.toStdString();
}

template<class T>
void OptionsCAO::setValue(const StandardKeys &key, const T &value)
{
    enumToKey(key) = value;
}

template<>
void OptionsCAO::setValue(const StandardKeys &key, const QString &value)
{
    enumToKey(key) = value.toStdString();
}

#ifdef GUI
void OptionsCAO::saveToUi(Ui::MainWindow *ui)
{
    QMutexLocker lock(mutex);

    //BSA
    ui->bsaExtractCheckBox->setChecked(getMandatoryValue<bool>(bBsaExtract));
    ui->bsaCreateCheckbox->setChecked(getMandatoryValue<bool>(bBsaCreate));
    ui->bsaDeleteBackupsCheckbox->setChecked(getMandatoryValue<bool>(bBsaDeleteBackup));

    //Textures
    const bool texturesOpt = getMandatoryValue<bool>(bTexturesMipmaps) || getMandatoryValue<bool>(bTexturesCompress)
                             || getMandatoryValue<bool>(bTexturesNecessary);
    if (!texturesOpt)
        ui->texturesGroupBox->setChecked(false);
    else
    {
        ui->texturesGroupBox->setChecked(true);
        ui->texturesNecessaryOptimizationCheckBox->setChecked(getMandatoryValue<bool>(bTexturesNecessary));
        ui->texturesCompressCheckBox->setChecked(getMandatoryValue<bool>(bTexturesCompress));
        ui->texturesMipmapCheckBox->setChecked(getMandatoryValue<bool>(bTexturesMipmaps));
    }

    //Textures resizing
    ui->texturesResizingGroupBox->setChecked(getMandatoryValue<bool>(bTexturesResizeSize)
                                             || getMandatoryValue<bool>(bTexturesResizeRatio));

    ui->texturesResizingBySizeRadioButton->setChecked(getMandatoryValue<bool>(bTexturesResizeSize));
    ui->texturesResizingBySizeWidth->setValue(getMandatoryValue<int>(iTexturesTargetWidth));
    ui->texturesResizingBySizeHeight->setValue(getMandatoryValue<int>(iTexturesTargetHeight));

    ui->texturesResizingByRatioRadioButton->setChecked(getMandatoryValue<bool>(bTexturesResizeRatio));
    ui->texturesResizingByRatioWidth->setValue(getMandatoryValue<int>(iTexturesTargetWidthRatio));
    ui->texturesResizingByRatioHeight->setValue(getMandatoryValue<int>(iTexturesTargetHeightRatio));

    //Meshes

    ui->meshesGroupBox->setChecked(true);
    switch (getMandatoryValue<uint>(iMeshesOptimizationLevel))
    {
        case 0: ui->meshesGroupBox->setChecked(false); break;
        case 1: ui->meshesNecessaryOptimizationRadioButton->setChecked(true); break;
        case 2: ui->meshesMediumOptimizationRadioButton->setChecked(true); break;
        case 3: ui->meshesFullOptimizationRadioButton->setChecked(true); break;
    }

    ui->meshesResaveCheckBox->setChecked(getMandatoryValue<bool>(bMeshesResave));
    ui->meshesHeadpartsCheckBox->setChecked(getMandatoryValue<bool>(bMeshesHeadparts));

    //Animations
    ui->animationsNecessaryOptimizationCheckBox->setChecked(getMandatoryValue<bool>(bAnimationsOptimization));

    //Log level
    ui->actionEnable_debug_log->setChecked(getMandatoryValue<bool>(bDebugLog));

    //General and GUI
    ui->dryRunCheckBox->setChecked(getMandatoryValue<bool>(bDryRun));
    ui->modeChooserComboBox->setCurrentIndex(
        ui->modeChooserComboBox->findData(getMandatoryValue<OptimizationMode>(eMode)));
    ui->userPathTextEdit->setText(getMandatoryValue<QString>(sUserPath));
}

void OptionsCAO::readFromUi(Ui::MainWindow *ui)
{
    QMutexLocker lock(mutex);

    //BSA
    const bool bsaEnabled = ui->bsaTab->isEnabled() && ui->bsaBaseGroupBox->isEnabled();
    setValue(bBsaExtract, bsaEnabled && ui->bsaExtractCheckBox->isChecked());
    setValue(bBsaCreate, bsaEnabled && ui->bsaCreateCheckbox->isChecked());
    setValue(bBsaDeleteBackup, bsaEnabled && ui->bsaDeleteBackupsCheckbox->isChecked());

    //Textures
    const bool texturesEnabled = ui->texturesGroupBox->isChecked() && ui->texturesTab->isEnabled();
    setValue(bTexturesNecessary, texturesEnabled && ui->texturesNecessaryOptimizationCheckBox->isChecked());
    setValue(bTexturesMipmaps, texturesEnabled && ui->texturesMipmapCheckBox->isChecked());
    setValue(bTexturesCompress, texturesEnabled && ui->texturesCompressCheckBox->isChecked());

    //Textures resizing
    const bool texturesResizing = ui->texturesResizingGroupBox->isChecked() && ui->texturesTab->isEnabled();
    setValue(bTexturesResizeSize, ui->texturesResizingBySizeRadioButton->isChecked() && texturesResizing);
    setValue(iTexturesTargetWidth, static_cast<size_t>(ui->texturesResizingBySizeWidth->value()));
    setValue(iTexturesTargetHeight, static_cast<size_t>(ui->texturesResizingBySizeHeight->value()));

    setValue(bTexturesResizeRatio, ui->texturesResizingByRatioRadioButton->isChecked() && texturesResizing);
    setValue(iTexturesTargetWidthRatio, static_cast<size_t>(ui->texturesResizingByRatioWidth->value()));
    setValue(iTexturesTargetHeightRatio, static_cast<size_t>(ui->texturesResizingByRatioHeight->value()));

    //Meshes base
    const bool meshesEnabled = ui->meshesTab->isEnabled();
    if (ui->meshesNecessaryOptimizationRadioButton->isChecked())
        setValue(iMeshesOptimizationLevel, 1);
    else if (ui->meshesMediumOptimizationRadioButton->isChecked())
        setValue(iMeshesOptimizationLevel, 2);
    else if (ui->meshesFullOptimizationRadioButton->isChecked())
        setValue(iMeshesOptimizationLevel, 3);
    if (!ui->meshesGroupBox->isChecked() || !meshesEnabled)
        setValue(iMeshesOptimizationLevel, 0);

    //Meshes advanced
    setValue(bMeshesHeadparts, meshesEnabled && ui->meshesHeadpartsCheckBox->isChecked());
    setValue(bMeshesResave, meshesEnabled && ui->meshesResaveCheckBox->isChecked());

    //Animations
    setValue(bAnimationsOptimization,
             ui->AnimationsTab->isEnabled() && ui->animationsNecessaryOptimizationCheckBox->isChecked());

    //General
    setValue(bDryRun, ui->dryRunCheckBox->isChecked());
    setValue(sUserPath, QDir::cleanPath(ui->userPathTextEdit->text()));
    setValue(eMode, ui->modeChooserComboBox->currentData().value<OptimizationMode>());
    setValue(bDebugLog, ui->actionEnable_debug_log->isChecked());
}
#endif

void OptionsCAO::parseArguments(const QStringList &args)
{
    QMutexLocker lock(mutex);

    if (args.count() < 4)
        throw std::runtime_error("Not enough arguments");
    QCommandLineParser parser;

    parser.addHelpOption();

    parser.addPositionalArgument("folder", "The folder to process, surrounded with quotes.");
    parser.addPositionalArgument("mode", "Either om (one mod) or sm (several mods)");
    parser.addPositionalArgument("profile", "One of the profile located in CAO/profiles");

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

        {"trr", "Enables textures resizing by ratio"},
        {"trrw", "The width ratio"},
        {"trrh", "The height ratio"},

        {"trs", "Enables textures resizing by fixed size"},
        {"trsw", "The width size"},
        {"trsh", "The height size"},

        {"a", "Enables animations processing"},
        {"mh", "Enables headparts detection and processing"},
        {"mr", "Enables meshes resaving"},
        {"be", "Enables BSA extraction."},
        {"bc", "Enables BSA creation."},
        {"bd", "Enables deletion of BSA backups."},
        /*{"bo",
         "NOT WORKING. Enables BSA optimization. The files inside the "
         "BSA will be extracted to memory and processed according to the provided settings "},*/
    });

    parser.process(args);

    const QString &path = QDir::cleanPath(parser.positionalArguments().at(0));
    userPath = path;

    const QString readMode = parser.positionalArguments().at(1);
    if (readMode == "om")
        mode = SingleMod;
    else if (readMode == "sm")
        mode = SeveralMods;
    else
        throw std::runtime_error("Invalid argument for mode");

    const QString &readGame = parser.positionalArguments().at(2);
    Profiles::setCurrentProfile(readGame);

    _json["General"]["bDryRun"] = parser.isSet("dr");
    _json["General"]["bDebugLog"] = parser.isSet("l");
    _json["General"]["mode"] = mode;
    _json["General"]["userPath"] = userPath.toStdString();

    //BSA
    _json["BSA"]["bBsaExtract"] = parser.isSet("be");
    _json["BSA"]["bBsaCreate"] = parser.isSet("bc");
    _json["BSA"]["bBsaDeleteBackup"] = parser.isSet("bd");
    _json["BSA"]["bBsaProcessContent"] = parser.isSet("bo");

    //Textures
    _json["Textures"]["bTexturesNecessary"] = parser.isSet("t0");
    _json["Textures"]["bTexturesCompress"] = parser.isSet("t1");
    _json["Textures"]["bTexturesMipmaps"] = parser.isSet("t2");

    _json["Textures"]["Resizing"]["bTexturesResizeSize"] = parser.isSet("trs");
    _json["Textures"]["Resizing"]["iTexturesTargetWidth"] = parser.value("trsw").toUInt();
    _json["Textures"]["Resizing"]["iTexturesTargetHeight"] = parser.value("trsh").toUInt();

    _json["Textures"]["Resizing"]["bTexturesResizeRatio"] = parser.isSet("trr");
    _json["Textures"]["Resizing"]["iTexturesTargetHeightRatio"] = parser.value("trrw").toUInt();
    _json["Textures"]["Resizing"]["iTexturesTargetWidthRatio"] = parser.value("trrh").toUInt();

    //Meshes
    _json["Meshes"]["iMeshesOptimizationLevel"] = parser.value("m").toInt();

    _json["Meshes"]["bMeshesHeadparts"] = parser.isSet("mh");
    _json["Meshes"]["bMeshesResave"] = parser.isSet("mr");

    //Animations
    _json["Animations"]["bAnimationsOptimization"] = parser.isSet("a");
}

const json &OptionsCAO::enumToKey(const StandardKeys &sKey) const
{
    const json *j = &_json;
    switch (sKey)
    {
        case bDebugLog: j = &j->at("General").at("bDebugLog");
        case bDryRun: j = &j->at("General").at("bDryRun");
        case eMode: j = &j->at("General").at("eMode");
        case sUserPath: j = &j->at("General").at("sUserPath");

        case bBsaExtract: j = &j->at("BSA").at("bBsaExtract");
        case bBsaCreate: j = &j->at("BSA").at("bBsaCreate");
        case bBsaDeleteBackup: j = &j->at("BSA").at("bBsaDeleteBackup");
        case bBsaProcessContent: j = &j->at("BSA").at("bBsaProcessContent");

        case bTexturesNecessary: j = &j->at("Textures").at("bTexturesNecessary");
        case bTexturesCompress: j = &j->at("Textures").at("bTexturesCompress");
        case bTexturesMipmaps: j = &j->at("Textures").at("bTexturesMipmaps");

        case bTexturesResizeSize: j = &j->at("Textures").at("Resizing").at("BySize").at("Enabled");
        case iTexturesTargetHeight: j = &j->at("Textures").at("Resizing").at("BySize").at("Width");
        case iTexturesTargetWidth: j = &j->at("Textures").at("Resizing").at("BySize").at("Height");

        case bTexturesResizeRatio: j = &j->at("Textures").at("Resizing").at("ByRatio").at("Enabled");
        case iTexturesTargetWidthRatio: j = &j->at("Textures").at("Resizing").at("ByRatio").at("Width");
        case iTexturesTargetHeightRatio: j = &j->at("Textures").at("Resizing").at("ByRatio").at("Height");

        case iMeshesOptimizationLevel: j = &j->at("Meshes").at("iMeshesOptimizationLevel");
        case bMeshesHeadparts: j = &j->at("Meshes").at("bMeshesHeadparts");
        case bMeshesResave: j = &j->at("Meshes").at("bMeshesResave");

        case bAnimationsOptimization: j = &j->at("Animations").at("bAnimationsOptimization");
    }
    return *j;
}

json &OptionsCAO::enumToKey(const StandardKeys &sKey)
{
    return const_cast<json &>(enumToKey(sKey));
}

const json &OptionsCAO::splitKey(const QString &key) const
{
    auto split = key.split("/");
    const json *j = &_json;
    for (const auto &subKey : split)
        j = &(*j)[subKey.toStdString()];

    return *j;
}

json &OptionsCAO::splitKey(const QString &key)
{
    return const_cast<json &>(splitKey(key));
}

const json &OptionsCAO::splitKeySafe(const QString &key) const
{
    auto split = key.split("/");
    const json *j = &_json;
    for (const auto &subKey : split)
        j = &j->at(subKey.toStdString());

    return *j;
}

json &OptionsCAO::splitKeySafe(const QString &key)
{
    return const_cast<json &>(splitKeySafe(key));
}

QString OptionsCAO::isValid() const
{
    if (!QDir(getMandatoryValue<QString>(sUserPath)).exists() || getMandatoryValue<QString>(sUserPath).size() < 5)
        return QString("This path does not exist or is shorter than 5 characters. Path: '%1'")
            .arg(getMandatoryValue<QString>(sUserPath));

    if (mode != SingleMod && mode != SeveralMods)
        return "This mode does not exist.";

    if (iMeshesOptimizationLevel < 0 || iMeshesOptimizationLevel > 3)
        return ("This meshes optimization level does not exist. Level: " + QString::number(iMeshesOptimizationLevel));

    if (getMandatoryValue<uint>(iTexturesTargetWidth) % 2 != 0
        || getMandatoryValue<uint>(iTexturesTargetHeight) % 2 != 0)
        return ("Textures target size has to be a power of two");

    return QString();
}

} // namespace CAO
