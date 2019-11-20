/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "AdvancedSettings.hpp"

namespace CAO {
namespace AdvancedSettings {

const nlohmann::json &toJSONKey(const AdvancedKey &key, const nlohmann::json &json)
{
    const nlohmann::json *j = &json;
    switch (key)
    {
        case bBSATabEnabled: j = &j->at("Advanced").at("BSA")["bBSATabEnabled"]; break;
        case sBSAExtension: j = &j->at("Advanced").at("BSA")["sBSAExtension"]; break;

        case eBSAFormat: j = &j->at("Advanced").at("BSA")["eBSAFormat"]; break;
        case iBSAMaxSize: j = &j->at("Advanced").at("BSA")["iBSAMaxSize"]; break;
        case sBSASuffix: j = &j->at("Advanced").at("BSA")["sBSASuffix"]; break;

        case bBSATexturesEnabled: j = &j->at("Advanced").at("BSA")["bBSATexturesEnabled"]; break;
        case eBSATexturesFormat: j = &j->at("Advanced").at("BSA")["eBSATexturesFormat"]; break;
        case iBSATexturesMaxSize: j = &j->at("Advanced").at("BSA")["iBSATexturesMaxSize"]; break;
        case sBSATexturesSuffix: j = &j->at("Advanced").at("BSA")["sBSATexturesSuffix"]; break;

        case bTexturesTabEnabled: j = &j->at("Advanced").at("Textures")["bTexturesTabEnabled"]; break;
        case eTexturesFormat: j = &j->at("Advanced").at("Textures")["eTexturesFormat"]; break;
        case bTexturesTGAConvertEnabled: j = &j->at("Advanced").at("Textures")["bTexturesTGAConvertEnabled"]; break;
        case bTexturesInterfaceConvert: j = &j->at("Advanced").at("Textures")["bTexturesInterfaceConvert"]; break;
        case slTexturesUnwantedFormats: j = &j->at("Advanced").at("Textures")["slTexturesUnwantedFormats"]; break;

        case bMeshesTabEnabled: j = &j->at("Advanced").at("Meshes")["bMeshesTabEnabled"]; break;
        case eMeshesFileVersion: j = &j->at("Advanced").at("Meshes")["eMeshesFileVersion"]; break;
        case iMeshesStream: j = &j->at("Advanced").at("Meshes")["iMeshesStream"]; break;
        case iMeshesUser: j = &j->at("Advanced").at("Meshes")["iMeshesUser"]; break;

        case bAnimationsTabEnabled: j = &j->at("Advanced").at("Animations")["bAnimationsTabEnabled"]; break;
        case eAnimationsFormat: j = &j->at("Advanced").at("Animations")["eAnimationsFormat"]; break;
    }
    return *j;
}

nlohmann::json &toJSONKey(const AdvancedKey &key, nlohmann::json &json)
{
    return const_cast<nlohmann::json &>(toJSONKey(std::as_const(key), std::as_const(json)));
}

#ifdef GUI
void saveToUi(Ui::MainWindow *ui, const nlohmann::json &json)
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

    auto getBool = [&json](const AdvancedKey &key) {
        auto &j = json;
        return toJSONKey(key, j).get<bool>();
    };

    auto getInt = [&json](const AdvancedKey &key) {
        auto &j = json;
        return toJSONKey(key, j).get<int>();
    };

    auto getString = [&json](const AdvancedKey &key) {
        auto &j = json;
        return QString::fromStdString(toJSONKey(key, j).get<std::string>());
    };

    auto getRawJson = [&json](const AdvancedKey &key) {
        auto &j = json;
        return toJSONKey(key, j);
    };

    iterateComboBox(ui->bsaFormat, getRawJson(eBSAFormat).get<bsa_archive_type_t>());
    iterateComboBox(ui->bsaTexturesFormat, getRawJson(eBSATexturesFormat).get<bsa_archive_type_t>());
    ui->bsaMaximumSize->setValue(getInt(iBSAMaxSize) / GigaByte);
    ui->bsaTexturesAdvancedGroupBox->setChecked(getBool(bBSATexturesEnabled));
    ui->bsaTexturesMaximumSize->setValue(getInt(iBSATexturesMaxSize) / GigaByte);
    ui->bsaExtension->setText(getString(sBSAExtension));
    ui->bsaSuffix->setText(getString(sBSASuffix));
    ui->bsaTexturesSuffix->setText(getString(sBSATexturesSuffix));

    iterateComboBox(ui->meshesUser, getInt(iMeshesUser));
    iterateComboBox(ui->meshesStream, getInt(iMeshesStream));
    iterateComboBox(ui->meshesVersion, getRawJson(eMeshesFileVersion).get<NiFileVersion>());

    //Animation format is not working when converting from amd64, thus not added to UI

    iterateComboBox(ui->texturesOutputFormat, getRawJson(eTexturesFormat).get<DXGI_FORMAT>());
    ui->texturesTgaConversionCheckBox->setChecked(getBool(bTexturesTGAConvertEnabled));
    ui->texturesCompressInterfaceCheckBox->setChecked(getBool(bTexturesInterfaceConvert));

    QStringList unwantedFormats;
    QVector<DXGI_FORMAT>();
    ui->texturesUnwantedFormatsList->clear();
    for (const DXGI_FORMAT &format : getRawJson(slTexturesUnwantedFormats).get<std::vector<DXGI_FORMAT>>())
        ui->texturesUnwantedFormatsList->addItem(dxgiFormatToString(format));
}

void readFromUi(Ui::MainWindow *ui, nlohmann::json &json)
{
    auto setValue = [&json](const AdvancedKey &key, const auto &value) {
        auto &j = json;
        toJSONKey(key, j) = value;
    };

    setValue(eBSAFormat, ui->bsaFormat->currentData().value<bsa_archive_type_e>());
    setValue(eBSATexturesFormat, ui->bsaTexturesFormat->currentData().value<bsa_archive_type_e>());

    setValue(iBSAMaxSize, ui->bsaMaximumSize->value() * GigaByte);
    setValue(bBSATexturesEnabled, ui->bsaTexturesAdvancedGroupBox->isChecked());
    setValue(iBSATexturesMaxSize, ui->bsaTexturesMaximumSize->value() * GigaByte);
    setValue(sBSAExtension, ui->bsaExtension->text().toStdString());
    setValue(sBSASuffix, ui->bsaSuffix->text().toStdString());
    setValue(sBSATexturesSuffix, ui->bsaTexturesSuffix->text().toStdString());

    setValue(iMeshesUser, ui->meshesUser->currentData().toUInt());
    setValue(iMeshesStream, ui->meshesStream->currentData().toUInt());
    setValue(eMeshesFileVersion, ui->meshesVersion->currentData().value<NiFileVersion>());
    //Animation format is not working currently, thus not added to UI

    setValue(eTexturesFormat, ui->texturesOutputFormat->currentData().value<DXGI_FORMAT>());
    setValue(bTexturesTGAConvertEnabled, ui->texturesTgaConversionCheckBox->isChecked());
    setValue(bTexturesInterfaceConvert, ui->texturesCompressInterfaceCheckBox->isChecked());

    QVector<DXGI_FORMAT> texUnwantedFmt;
    for (int i = 0; i < ui->texturesUnwantedFormatsList->count(); ++i)
    {
        const auto &entry = ui->texturesUnwantedFormatsList->item(i);
        const DXGI_FORMAT &format = stringToDxgiFormat(entry->text());
        if (!texUnwantedFmt.contains(format) && format != DXGI_FORMAT_UNKNOWN)
            texUnwantedFmt += format;
    }
    setValue(slTexturesUnwantedFormats, texUnwantedFmt.toStdVector());
}
#endif

} // namespace AdvancedSettings
} // namespace CAO
