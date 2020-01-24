/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "BaseTypes.hpp"
#include "JSON.hpp"
#include "UISync.hpp"
#include "ui_BSAFilesToPack.h"
#include "ui_mainWindow.h"

/*
 * Beware that the code in this file is quite messy
 */

namespace CAO {

using uiReadMW = void (*)(const Ui::MainWindow &ui, JSON &json);
using uiSaveMW = void (*)(Ui::MainWindow &ui, const JSON &json);
using uiReadBSAFiles = void (*)(const Ui::BSAFilesToPack &ui, JSON &json);
using uiSaveBSAFiles = void (*)(Ui::BSAFilesToPack &ui, const JSON &json);

class Setting
{
private:
    enum class TargetUI
    {
        bsaUi,
        mainUi
    } _targetUI;

    uiReadMW _readMW;
    uiReadBSAFiles _readBSA;
    uiSaveMW _saveMW;
    uiSaveBSAFiles _saveBSA;

public:
    Setting(const QString &key, uiReadMW readMW, uiSaveMW saveMW)
        : _targetUI(TargetUI::mainUi)
        , _readMW(readMW)
        , _saveMW(saveMW)
        , jsonKey(key)
    {}

    Setting(const QString &key, uiReadBSAFiles readBSA, uiSaveBSAFiles saveBSA)
        : _targetUI(TargetUI::bsaUi)
        , _readBSA(readBSA)
        , _saveBSA(saveBSA)
        , jsonKey(key)

    {}

    QString jsonKey;

    void readFromUI(const Ui::MainWindow &ui, const Ui::BSAFilesToPack &bsUi, JSON &json) const
    {
        switch (_targetUI)
        {
            case TargetUI::bsaUi: _readBSA(bsUi, json); break;
            case TargetUI::mainUi: _readMW(ui, json);
        }
    }
    void saveToUI(Ui::MainWindow &ui, Ui::BSAFilesToPack &bsUi, const JSON &json) const
    {
        switch (_targetUI)
        {
            case TargetUI::bsaUi: _saveBSA(bsUi, json); break;
            case TargetUI::mainUi: _saveMW(ui, json);
        }
    }
};

// Used to register settings into a list
struct settingBuilder
{
    settingBuilder(QVector<const Setting *> &vec, const Setting *setting) { vec << setting; } // namespace CAO
};

#define REGISTER_SETTING(name, key, readFunc, saveFunc) \
    static const Setting name(key, readFunc, saveFunc); \
    static SettingBuilder builder_##name(settingsList, &name);

#define REGISTER_SETTING_MW(name, key, uiElement, readFunc, saveFunc) \
    REGISTER_SETTING( \
        name, \
        key, \
        [](const Ui::MainWindow &ui, JSON &json) { \
            json.setValue(key, UISync::readFunc(uiElement)); \
        }, \
        [](Ui::MainWindow &ui, const JSON &json) { \
            UISync::saveFunc(uiElement, \
                             json.getValue<decltype( \
                                 UISync::readFunc(std::declval<decltype(uiElement)>()))>(key)); \
        })

#define REGISTER_SETTING_CHECKBOX_MW(name, key, uiElement) \
    REGISTER_SETTING_MW(name, key, uiElement, readCheckbox, saveCheckbox)

#define REGISTER_SETTING_RADIO_MW(name, key, uiElement, parent) \
    REGISTER_SETTING( \
        name, \
        key, \
        [](const Ui::MainWindow &ui, JSON &json) { \
            json.setValue(key, UISync::readCheckbox(uiElement, parent)); \
        }, \
        [](Ui::MainWindow &ui, const JSON &json) { \
            UISync::saveCheckbox(uiElement, json.getValue<bool>(key)); \
        })

#define REGISTER_SETTING_BSW(name, key, uiElement, readFunc, saveFunc) \
    REGISTER_SETTING( \
        name, \
        key, \
        [](const Ui::BSAFilesToPack &ui, JSON &json) { \
            json.setValue(key, UISync::readFunc(uiElement)); \
        }, \
        [](Ui::BSAFilesToPack &ui, const JSON &json) { \
            UISync::saveFunc(uiElement, \
                             json.getValue<decltype( \
                                 UISync::readFunc(std::declval<decltype(uiElement)>()))>(key)); \
        })

#define REGISTER_SETTING_NO_UI(name, key) \
    static const Setting name( \
        key, \
        []([[maybe_unused]] const Ui::MainWindow &ui, [[maybe_unused]] JSON &json) { return; }, \
        []([[maybe_unused]] Ui::MainWindow &ui, [[maybe_unused]] const JSON &json) { return; });

// clang-format off
      static QVector<const Setting*> settingsList;

      REGISTER_SETTING_CHECKBOX_MW(bDebugLog,
          "General/bDebugLog",
          ui.actionEnable_debug_log)

      REGISTER_SETTING_CHECKBOX_MW(bDryRun,
          "General/bDryRun",
          ui.dryRunCheckBox)

      REGISTER_SETTING_MW(eMode,
          "General/eMode",
          ui.modeChooserComboBox,
          readValue<OptimizationMode>,
          saveValue)

      REGISTER_SETTING_MW(sUserPath,
          "General/sUserPath",
          ui.userPathTextEdit,
          readText,
          saveText)

      REGISTER_SETTING_CHECKBOX_MW(bBsaExtract,
          "BSA/bBsaExtract",
          ui.bsaExtractCheckBox)

      REGISTER_SETTING_CHECKBOX_MW(bBsaCreate,
          "BSA/bBsaCreate",
          ui.bsaCreateCheckbox)

      REGISTER_SETTING_CHECKBOX_MW(bBsaDeleteBackup,
          "BSA/bBsaDeleteBackup",
          ui.bsaDeleteBackupsCheckBox)

      REGISTER_SETTING_CHECKBOX_MW(bBsaLeastBsaPossible,
          "BSA/bBsaLeastBsaPossible",
          ui.bsaLeastBsasCheckBox)

      REGISTER_SETTING_CHECKBOX_MW(bBsaCreateDummies,
          "BSA/bBsaCreateDummies",
          ui.bsaCreateDummiesCheckBox)

      REGISTER_SETTING_CHECKBOX_MW(bTexturesNecessary,
          "Textures/bTexturesNecessary",
          ui.texturesNecessaryOptimizationCheckBox)

      REGISTER_SETTING_CHECKBOX_MW(bTexturesCompress,
          "Textures/bTexturesCompress",
          ui.texturesCompressCheckBox)

      REGISTER_SETTING_CHECKBOX_MW(bTexturesMipmaps,
          "Textures/bTexturesMipmaps",
          ui.texturesMipmapCheckBox)

      REGISTER_SETTING_RADIO_MW(bTexturesResizeSize,
          "Textures/Resizing/BySize/Enabled",
          ui.texturesResizingBySizeRadioButton,
          ui.texturesResizingGroupBox)
          

      REGISTER_SETTING_MW(iTexturesTargetHeight,
          "Textures/Resizing/BySize/Height",
          ui.texturesResizingBySizeHeight,
          readValue,
          saveValue)

      REGISTER_SETTING_MW(iTexturesTargetWidth,
          "Textures/Resizing/BySize/Width",
          ui.texturesResizingBySizeHeight,
          readValue,
          saveValue)

      REGISTER_SETTING_RADIO_MW(bTexturesResizeRatio,
          "Textures/Resizing/ByRatio/Enabled",
          ui.texturesResizingByRatioRadioButton,
          ui.texturesResizingGroupBox)

      REGISTER_SETTING_MW(iTexturesTargetWidthRatio,
          "Textures/Resizing/ByRatio/Width",
          ui.texturesResizingByRatioWidth,
          readValue,
          saveValue)

      REGISTER_SETTING_MW(iTexturesTargetHeightRatio,
          "Textures/Resizing/ByRatio/Height",
          ui.texturesResizingByRatioHeight,
          readValue,
          saveValue)
/*FIXME
      REGISTER_SETTING_MW(iMeshesOptimizationLevel, 
          "Meshes/iMeshesOptimizationLevel",
          ui,
          readMeshesOptLevel,
          saveMeshesOptLevel)
*/

REGISTER_SETTING_NO_UI(iMeshesOptimizationLevel, "Meshes/iMeshesOptimizationLevel")


      REGISTER_SETTING_CHECKBOX_MW(bAnimationsOptimization,
          "Animations/bAnimationsOptimization",
          ui.animationsNecessaryOptimizationCheckBox)

      REGISTER_SETTING_CHECKBOX_MW(bMeshesHeadparts,
          "Meshes/bMeshesHeadparts",
          ui.meshesHeadpartsCheckBox)

      REGISTER_SETTING_CHECKBOX_MW(bMeshesResave,
          "Meshes/bMeshesResave",
          ui.meshesResaveCheckBox)

      REGISTER_SETTING_MW(bBSATabEnabled,
          "Advanced/BSA/bBSATabEnabled",
          ui.bsaTab,
          readTab,
          saveTab)

      REGISTER_SETTING_MW(sBSAExtension,
          "Advanced/BSA/sBSAExtension",
          ui.bsaExtension,
          readText,
          saveText)

      REGISTER_SETTING_MW(eBSAFormat,
          "Advanced/BSA/eBSAFormat",
          ui.bsaFormat,
          readValue<bsa_archive_type_t>,
          saveValue)

      REGISTER_SETTING_MW(iBSAMaxSize,
          "Advanced/BSA/iBSAMaxSize",
          ui.bsaMaximumSize,
          readGigaByteValue,
          saveValueGigabyte)

      REGISTER_SETTING_MW(sBSASuffix,
          "Advanced/BSA/sBSASuffix",
          ui.bsaSuffix,
          readText,
          saveText)

      REGISTER_SETTING_CHECKBOX_MW(bBSATexturesEnabled,
          "Advanced/BSA/bBSATexturesEnabled",
          ui.bsaTexturesAdvancedGroupBox)

      REGISTER_SETTING_MW(eBSATexturesFormat,
          "Advanced/BSA/eBSATexturesFormat",
          ui.bsaTexturesFormat,
          readValue<bsa_archive_type_t>,
          saveValue)

      REGISTER_SETTING_MW(iBSATexturesMaxSize,
          "Advanced/BSA/iBSATexturesMaxSize",
          ui.bsaTexturesMaximumSize,
          readGigaByteValue,
          saveValueGigabyte)

      REGISTER_SETTING_MW(sBSATexturesSuffix,
          "Advanced/BSA/sBSATexturesSuffix",
          ui.bsaTexturesSuffix,
          readText,
          saveText)

      REGISTER_SETTING_BSW(slBSAStandardExt,
          "Advanced/BSA/slBSAStandardExt",      
          ui.StandardFilesListWidget,
          readList<std::string>,
          saveList)

      REGISTER_SETTING_BSW(slBSATexturesExt,
          "Advanced/BSA/slBSATexturesExt",
          ui.TexturesFilesListWidget,
          readList<std::string>,
          saveList)

      REGISTER_SETTING_BSW(slBSAUncompressableExt,
          "Advanced/BSA/slBSAUncompressableExt",
          ui.UncompressableFilesListWidget,
          readList<std::string>,
          saveList)

      REGISTER_SETTING_MW(bTexturesTabEnabled,
          "Advanced/Textures/bTexturesTabEnabled",
          ui.texturesTab,
          readTab,
          saveTab)

      REGISTER_SETTING_MW(eTexturesFormat,
          "Advanced/Textures/eTexturesFormat",
          ui.texturesOutputFormat,
          readValue<DXGI_FORMAT>,
          saveValue)

      REGISTER_SETTING_CHECKBOX_MW(bTexturesTGAConvertEnabled,
          "Advanced/Textures/bTexturesTGAConvertEnabled",
          ui.texturesTgaConversionCheckBox)

      REGISTER_SETTING_CHECKBOX_MW(bTexturesInterfaceConvert,
          "Advanced/Textures/bTexturesInterfaceConvert",
          ui.texturesCompressInterfaceCheckBox)

      REGISTER_SETTING_MW(slTexturesUnwantedFormats,
          "Advanced/Textures/slTexturesUnwantedFormats",
          ui.texturesUnwantedFormatsList,
          readList<DXGI_FORMAT>,
          saveList)

      REGISTER_SETTING_MW(bMeshesTabEnabled,
          "Advanced/Meshes/bMeshesTabEnabled",
          ui.meshesTab,
          readTab,
          saveTab)

      REGISTER_SETTING_MW(eMeshesFileVersion,
          "Advanced/Meshes/eMeshesFileVersion",
          ui.meshesVersion,
          readValue<NiFileVersion>,
          saveValue)

      REGISTER_SETTING_MW(iMeshesStream,
          "Advanced/Meshes/iMeshesStream",
          ui.meshesStream,
          readValue<int>,
          saveValue)

      REGISTER_SETTING_MW(iMeshesUser,
          "Advanced/Meshes/iMeshesUser",
          ui.meshesUser,
          readValue<int>,
          saveValue)

      REGISTER_SETTING_MW(bAnimationsTabEnabled,
          "Advanced/Animations/bAnimationsTabEnabled",
          ui.AnimationsTab,
          readTab,
          saveTab)

      REGISTER_SETTING_NO_UI(eAnimationsFormat,
          "Advanced/Animations/eAnimationsFormat")

// clang-format on
#undef REGISTER_SETTING
#undef REGISTER_SETTING_MW
#undef REGISTER_SETTING_CHECKBOX_MW
#undef REGISTER_SETTING_RADIO_MW
#undef REGISTER_SETTING_BSW
#undef REGISTER_SETTING_NO_UI

} // namespace CAO
