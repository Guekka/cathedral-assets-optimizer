/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "StandardSettings.hpp"

namespace CAO {
namespace StandardSettings {
const json &toJSONKey(const StandardKey &key, const json &json)
{
    nlohmann::json *j = const_cast<nlohmann::json *>(&json);
    switch (key)
    {
        case bDebugLog: j = &j->at("General")["bDebugLog"]; break;
        case bDryRun: j = &j->at("General")["bDryRun"]; break;
        case eMode: j = &j->at("General")["eMode"]; break;
        case sUserPath: j = &j->at("General")["sUserPath"]; break;

        case bBsaExtract: j = &j->at("BSA")["bBsaExtract"]; break;
        case bBsaCreate: j = &j->at("BSA")["bBsaCreate"]; break;
        case bBsaDeleteBackup: j = &j->at("BSA")["bBsaDeleteBackup"]; break;
        case bBsaProcessContent: j = &j->at("BSA")["bBsaProcessContent"]; break;

        case bTexturesNecessary: j = &j->at("Textures")["bTexturesNecessary"]; break;
        case bTexturesCompress: j = &j->at("Textures")["bTexturesCompress"]; break;
        case bTexturesMipmaps: j = &j->at("Textures")["TexturesMipmaps"]; break;

        case bTexturesResizeSize: j = &j->at("Textures").at("Resizing").at("BySize")["Enabled"]; break;
        case iTexturesTargetHeight: j = &j->at("Textures").at("Resizing").at("BySize")["Width"]; break;
        case iTexturesTargetWidth: j = &j->at("Textures").at("Resizing").at("BySize")["Height"]; break;

        case bTexturesResizeRatio: j = &j->at("Textures").at("Resizing").at("ByRatio")["Enabled"]; break;
        case iTexturesTargetWidthRatio: j = &j->at("Textures").at("Resizing").at("ByRatio")["Width"]; break;
        case iTexturesTargetHeightRatio: j = &j->at("Textures").at("Resizing").at("ByRatio")["Height"]; break;

        case iMeshesOptimizationLevel: j = &j->at("Meshes")["iMeshesOptimizationLevel"]; break;
        case bMeshesHeadparts: j = &j->at("Meshes")["bMeshesHeadparts"]; break;
        case bMeshesResave: j = &j->at("Meshes")["bMeshesResave"]; break;

        case bAnimationsOptimization: j = &j->at("Animations")["bAnimationsOptimization"]; break;
    }
    return *j;
}

json &toJSONKey(const StandardKey &key, json &json)
{
    return const_cast<nlohmann::json &>(toJSONKey(std::as_const(key), std::as_const(json)));
}

#ifdef GUI
void saveToUi(Ui::MainWindow *ui, const json &json)
{
    auto getBool = [&json](const StandardKey &key) {
        auto &j = json;
        return toJSONKey(key, j).get<bool>();
    };

    auto getInt = [&json](const StandardKey &key) {
        auto &j = json;
        return toJSONKey(key, j).get<int>();
    };

    auto getRawJson = [&json](const StandardKey &key) {
        auto &j = json;
        return toJSONKey(key, j);
    };

    //BSA
    ui->bsaExtractCheckBox->setChecked(getBool(bBsaExtract));
    ui->bsaCreateCheckbox->setChecked(getBool(bBsaCreate));
    ui->bsaDeleteBackupsCheckbox->setChecked(getBool(bBsaDeleteBackup));

    //Textures
    const bool texturesOpt = getBool(bTexturesMipmaps) || getBool(bTexturesCompress) || getBool(bTexturesNecessary);
    if (!texturesOpt)
        ui->texturesGroupBox->setChecked(false);
    else
    {
        ui->texturesGroupBox->setChecked(true);
        ui->texturesNecessaryOptimizationCheckBox->setChecked(getBool(bTexturesNecessary));
        ui->texturesCompressCheckBox->setChecked(getBool(bTexturesCompress));
        ui->texturesMipmapCheckBox->setChecked(getBool(bTexturesMipmaps));
    }

    //Textures resizing
    ui->texturesResizingGroupBox->setChecked(getBool(bTexturesResizeSize) || getBool(bTexturesResizeRatio));

    ui->texturesResizingBySizeRadioButton->setChecked(getBool(bTexturesResizeSize));
    ui->texturesResizingBySizeWidth->setValue(getInt(iTexturesTargetWidth));
    ui->texturesResizingBySizeHeight->setValue(getInt(iTexturesTargetHeight));

    ui->texturesResizingByRatioRadioButton->setChecked(getBool(bTexturesResizeRatio));
    ui->texturesResizingByRatioWidth->setValue(getInt(iTexturesTargetWidthRatio));
    ui->texturesResizingByRatioHeight->setValue(getInt(iTexturesTargetHeightRatio));

    //Meshes

    ui->meshesGroupBox->setChecked(true);
    switch (getRawJson(iMeshesOptimizationLevel).get<uint>())
    {
        case 0: ui->meshesGroupBox->setChecked(false); break;
        case 1: ui->meshesNecessaryOptimizationRadioButton->setChecked(true); break;
        case 2: ui->meshesMediumOptimizationRadioButton->setChecked(true); break;
        case 3: ui->meshesFullOptimizationRadioButton->setChecked(true); break;
    }

    ui->meshesResaveCheckBox->setChecked(getBool(bMeshesResave));
    ui->meshesHeadpartsCheckBox->setChecked(getBool(bMeshesHeadparts));

    //Animations
    ui->animationsNecessaryOptimizationCheckBox->setChecked(getBool(bAnimationsOptimization));

    //Log level
    ui->actionEnable_debug_log->setChecked(getBool(bDebugLog));

    //General and GUI
    ui->dryRunCheckBox->setChecked(getBool(bDryRun));
    ui->modeChooserComboBox->setCurrentIndex(
        ui->modeChooserComboBox->findData(getRawJson(eMode).get<OptimizationMode>()));
    ui->userPathTextEdit->setText(QString::fromStdString(getRawJson(sUserPath).get<std::string>()));
}

void readFromUi(Ui::MainWindow *ui, json &json)
{
    auto setValue = [&json](const StandardKey &key, const auto &value) {
        auto &j = json;
        toJSONKey(key, j) = value;
    };

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
    setValue(sUserPath, QDir::cleanPath(ui->userPathTextEdit->text()).toStdString());
    setValue(eMode, ui->modeChooserComboBox->currentData().value<OptimizationMode>());
    setValue(bDebugLog, ui->actionEnable_debug_log->isChecked());
}
#endif

} // namespace StandardSettings
} // namespace CAO
