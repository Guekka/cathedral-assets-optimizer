/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once
#ifdef dqzihud

#include "Settings/Settings.hpp"

namespace CAO {
namespace {

void saveToUiStandard(Ui::MainWindow &ui, const Settings &settings)
{
    auto getBool = [&settings](const StandardKey &key) { return settings.getValue<bool>(key); };
    auto getInt = [&settings](const StandardKey &key) { return settings.getValue<int>(key); };

    //BSA
    ui.bsaExtractCheckBox->setChecked(getBool(bBsaExtract));
    ui.bsaCreateCheckbox->setChecked(getBool(bBsaCreate));
    ui.bsaDeleteBackupsCheckBox->setChecked(getBool(bBsaDeleteBackup));
    ui.bsaLeastBsasCheckBox->setChecked(getBool(bBsaLeastBsaPossible));
    ui.bsaCreateDummiesCheckBox->setChecked(getBool(bBsaCreateDummies));

    //Textures
    const bool texturesOpt = getBool(bTexturesMipmaps) || getBool(bTexturesCompress) || getBool(bTexturesNecessary);
    if (!texturesOpt)
        ui.texturesGroupBox->setChecked(false);
    else
    {
        ui.texturesGroupBox->setChecked(true);
        ui.texturesNecessaryOptimizationCheckBox->setChecked(getBool(bTexturesNecessary));
        ui.texturesCompressCheckBox->setChecked(getBool(bTexturesCompress));
        ui.texturesMipmapCheckBox->setChecked(getBool(bTexturesMipmaps));
    }

    //Textures resizing
    ui.texturesResizingGroupBox->setChecked(getBool(bTexturesResizeSize) || getBool(bTexturesResizeRatio));

    ui.texturesResizingBySizeRadioButton->setChecked(getBool(bTexturesResizeSize));
    ui.texturesResizingBySizeWidth->setValue(getInt(iTexturesTargetWidth));
    ui.texturesResizingBySizeHeight->setValue(getInt(iTexturesTargetHeight));

    ui.texturesResizingByRatioRadioButton->setChecked(getBool(bTexturesResizeRatio));
    ui.texturesResizingByRatioWidth->setValue(getInt(iTexturesTargetWidthRatio));
    ui.texturesResizingByRatioHeight->setValue(getInt(iTexturesTargetHeightRatio));

    //Meshes
    ui.meshesGroupBox->setChecked(true);
    switch (settings.getValue<uint>(iMeshesOptimizationLevel))
    {
        case 0: ui.meshesGroupBox->setChecked(false); break;
        case 1: ui.meshesNecessaryOptimizationRadioButton->setChecked(true); break;
        case 2: ui.meshesMediumOptimizationRadioButton->setChecked(true); break;
        case 3: ui.meshesFullOptimizationRadioButton->setChecked(true); break;
    }

    ui.meshesResaveCheckBox->setChecked(getBool(bMeshesResave));
    ui.meshesHeadpartsCheckBox->setChecked(getBool(bMeshesHeadparts));

    //Animations
    ui.animationsNecessaryOptimizationCheckBox->setChecked(getBool(bAnimationsOptimization));

    //Log level
    ui.actionEnable_debug_log->setChecked(getBool(bDebugLog));

    //General and GUI
    ui.dryRunCheckBox->setChecked(getBool(bDryRun));
    ui.modeChooserComboBox->setCurrentIndex(
        ui.modeChooserComboBox->findData(settings.getValue<OptimizationMode>(eMode)));
    ui.userPathTextEdit->setText(settings.getValue<QString>(sUserPath));
}

void readFromUiStandard(Ui::MainWindow &ui, Settings &settings)
{
    auto setValue = [&settings](const StandardKey &key, const auto &value) { settings.setValue(key, value); };

    //BSA
    const bool bsaEnabled = ui.bsaTab->isEnabled() && ui.bsaBaseGroupBox->isEnabled();
    setValue(bBsaExtract, bsaEnabled && ui.bsaExtractCheckBox->isChecked());
    setValue(bBsaCreate, bsaEnabled && ui.bsaCreateCheckbox->isChecked());
    setValue(bBsaDeleteBackup, bsaEnabled && ui.bsaDeleteBackupsCheckBox->isChecked());
    setValue(bBsaLeastBsaPossible, bsaEnabled && ui.bsaLeastBsasCheckBox->isChecked());
    setValue(bBsaCreateDummies, bsaEnabled && ui.bsaCreateDummiesCheckBox->isChecked());

    //Textures
    const bool texturesEnabled = ui.texturesGroupBox->isChecked() && ui.texturesTab->isEnabled();
    setValue(bTexturesNecessary, texturesEnabled && ui.texturesNecessaryOptimizationCheckBox->isChecked());
    setValue(bTexturesMipmaps, texturesEnabled && ui.texturesMipmapCheckBox->isChecked());
    setValue(bTexturesCompress, texturesEnabled && ui.texturesCompressCheckBox->isChecked());

    //Textures resizing
    const bool texturesResizing = ui.texturesResizingGroupBox->isChecked() && ui.texturesTab->isEnabled();
    setValue(bTexturesResizeSize, ui.texturesResizingBySizeRadioButton->isChecked() && texturesResizing);
    setValue(iTexturesTargetWidth, static_cast<size_t>(ui.texturesResizingBySizeWidth->value()));
    setValue(iTexturesTargetHeight, static_cast<size_t>(ui.texturesResizingBySizeHeight->value()));

    setValue(bTexturesResizeRatio, ui.texturesResizingByRatioRadioButton->isChecked() && texturesResizing);
    setValue(iTexturesTargetWidthRatio, static_cast<size_t>(ui.texturesResizingByRatioWidth->value()));
    setValue(iTexturesTargetHeightRatio, static_cast<size_t>(ui.texturesResizingByRatioHeight->value()));

    //Meshes base
    const bool meshesEnabled = ui.meshesTab->isEnabled();
    if (ui.meshesNecessaryOptimizationRadioButton->isChecked())
        setValue(iMeshesOptimizationLevel, 1);
    else if (ui.meshesMediumOptimizationRadioButton->isChecked())
        setValue(iMeshesOptimizationLevel, 2);
    else if (ui.meshesFullOptimizationRadioButton->isChecked())
        setValue(iMeshesOptimizationLevel, 3);
    if (!ui.meshesGroupBox->isChecked() || !meshesEnabled)
        setValue(iMeshesOptimizationLevel, 0);

    //Meshes advanced
    setValue(bMeshesHeadparts, meshesEnabled && ui.meshesHeadpartsCheckBox->isChecked());
    setValue(bMeshesResave, meshesEnabled && ui.meshesResaveCheckBox->isChecked());

    //Animations
    setValue(bAnimationsOptimization,
             ui.AnimationsTab->isEnabled() && ui.animationsNecessaryOptimizationCheckBox->isChecked());

    //General
    setValue(bDryRun, ui.dryRunCheckBox->isChecked());
    setValue(sUserPath, QDir::cleanPath(ui.userPathTextEdit->text()).toStdString());
    setValue(eMode, ui.modeChooserComboBox->currentData().value<OptimizationMode>());
    setValue(bDebugLog, ui.actionEnable_debug_log->isChecked());
}

void saveToUiAdvanced(Ui::MainWindow &ui, const Settings &settings)
{
    const auto iterateComboBox = [](QComboBox *box, const QVariant data) {
        for (int i = 0; i < box->count(); ++i)
        {
            if (box->itemData(i) == data)
            {
                box->setCurrentIndex(i);
                break;
            }
        }
    };

    auto getBool = [&settings](const AdvancedKey &key) { return settings.getValue<bool>(key); };
    auto getInt = [&settings](const AdvancedKey &key) { return settings.getValue<int>(key); };
    auto getString = [&settings](const AdvancedKey &key) { return settings.getValue<QString>(key); };

    iterateComboBox(ui.bsaFormat, settings.getValue<bsa_archive_type_t>(eBSAFormat));
    iterateComboBox(ui.bsaTexturesFormat, settings.getValue<bsa_archive_type_t>((eBSATexturesFormat)));
    ui.bsaMaximumSize->setValue(settings.getValue<double>(iBSAMaxSize) / GigaByte);
    ui.bsaTexturesAdvancedGroupBox->setChecked(getBool(bBSATexturesEnabled));
    ui.bsaTexturesMaximumSize->setValue(settings.getValue<double>(iBSATexturesMaxSize) / GigaByte);
    ui.bsaExtension->setText(getString(sBSAExtension));
    ui.bsaSuffix->setText(getString(sBSASuffix));
    ui.bsaTexturesSuffix->setText(getString(sBSATexturesSuffix));

    iterateComboBox(ui.meshesUser, getInt(iMeshesUser));
    iterateComboBox(ui.meshesStream, getInt(iMeshesStream));
    iterateComboBox(ui.meshesVersion, settings.getValue<NiFileVersion>((eMeshesFileVersion)));

    //Animation format is not working when converting from amd64, thus not added to UI

    iterateComboBox(ui.texturesOutputFormat, settings.getValue<DXGI_FORMAT>((eTexturesFormat)));
    ui.texturesTgaConversionCheckBox->setChecked(getBool(bTexturesTGAConvertEnabled));
    ui.texturesCompressInterfaceCheckBox->setChecked(getBool(bTexturesInterfaceConvert));

    QStringList unwantedFormats;
    ui.texturesUnwantedFormatsList->clear();
    for (const DXGI_FORMAT &format : settings.getValue<std::vector<DXGI_FORMAT>>((slTexturesUnwantedFormats)))
        ui.texturesUnwantedFormatsList->addItem(dxgiFormatToString(format));
}

void readFromUiAdvanced(Ui::MainWindow &ui, CAO::Settings &settings)
{
    auto setValue = [&settings](const AdvancedKey &key, const auto &value) { settings.setValue(key, value); };

    auto temp = ui.bsaFormat->currentData().value<bsa_archive_type_e>();
    setValue(eBSAFormat, ui.bsaFormat->currentData().value<bsa_archive_type_e>());
    setValue(eBSATexturesFormat, ui.bsaTexturesFormat->currentData().value<bsa_archive_type_e>());

    setValue(iBSAMaxSize, ui.bsaMaximumSize->value() * GigaByte);
    setValue(bBSATexturesEnabled, ui.bsaTexturesAdvancedGroupBox->isChecked());
    setValue(iBSATexturesMaxSize, ui.bsaTexturesMaximumSize->value() * GigaByte);
    setValue(sBSAExtension, ui.bsaExtension->text().toStdString());
    setValue(sBSASuffix, ui.bsaSuffix->text().toStdString());
    setValue(sBSATexturesSuffix, ui.bsaTexturesSuffix->text().toStdString());

    setValue(iMeshesUser, ui.meshesUser->currentData().toUInt());
    setValue(iMeshesStream, ui.meshesStream->currentData().toUInt());
    setValue(eMeshesFileVersion, ui.meshesVersion->currentData().value<NiFileVersion>());
    //Animation format is not working currently, thus not added to UI

    setValue(eTexturesFormat, ui.texturesOutputFormat->currentData().value<DXGI_FORMAT>());
    setValue(bTexturesTGAConvertEnabled, ui.texturesTgaConversionCheckBox->isChecked());
    setValue(bTexturesInterfaceConvert, ui.texturesCompressInterfaceCheckBox->isChecked());

    QVector<DXGI_FORMAT> texUnwantedFmt;
    for (int i = 0; i < ui.texturesUnwantedFormatsList->count(); ++i)
    {
        const auto &entry = ui.texturesUnwantedFormatsList->item(i);
        const DXGI_FORMAT &format = stringToDxgiFormat(entry->text());
        if (!texUnwantedFmt.contains(format) && format != DXGI_FORMAT_UNKNOWN)
            texUnwantedFmt += format;
    }
    setValue(slTexturesUnwantedFormats, texUnwantedFmt.toStdVector());
}
} // namespace

void readFromUI(Ui::MainWindow &ui, CAO::Settings &settings)
{
    readFromUiAdvanced(ui, settings);
    readFromUiStandard(ui, settings);
}

void saveToUI(Ui::MainWindow &ui, CAO::Settings &settings)
{
    saveToUiAdvanced(ui, settings);
    saveToUiStandard(ui, settings);
}

} // namespace CAO
#endif
