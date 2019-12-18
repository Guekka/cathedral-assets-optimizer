/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "BaseTypes.hpp"
#include "JSON.hpp"
#include "ui_BSAFilesToPack.h"
#include "ui_mainWindow.h"

/*
 * Beware that the code in this file is quite messy
 */

namespace CAO {
template<typename returnValue, typename Widget>
inline returnValue readUiData(const Widget *widget, const void *context = nullptr);

template<>
inline bool readUiData(const QCheckBox *widget, const void *context)
{
    return widget->isChecked();
}

template<>
inline bool readUiData(const QAction *widget, const void *context)
{
    return widget->isChecked();
}

template<>
inline bool readUiData(const QRadioButton *widget, const void *context)
{
    auto parentGroupBox = context ? static_cast<const QGroupBox *>(context) : nullptr;
    if (parentGroupBox)
        if (!parentGroupBox->isChecked())
            return false;
    return widget->isChecked();
}

template<>
inline bool readUiData(const QGroupBox *widget, const void *context)
{
    return widget->isChecked();
}

template<>
// NOTE Overload used for meshes optimization level. Very dependant of the UI layout.
inline int readUiData(const QGroupBox *widget, const void *context)
{
    if (!widget->isChecked())
        return 0;
    auto widgetList = widget->findChildren<QRadioButton *>();
    int counter = 0;
    for (auto child : widgetList)
    {
        counter++;
        if (child->isChecked())
            return counter;
    }
    return 0;
}

template<>
inline bool readUiData(const QWidget *widget, const void *context)
{
    return widget->isEnabled();
}

template<>
inline int readUiData(const QSpinBox *widget, const void *context)
{
    return widget->value();
}

template<>
inline double readUiData(const QDoubleSpinBox *widget, const void *context)
{
    double mul = context ? *static_cast<const double *>(context) : 1.0;
    return widget->value() * mul;
}

template<>
inline std::string readUiData(const QLineEdit *widget, const void *context)
{
    return widget->text().toStdString();
}

template<typename returnValue>
inline returnValue readUiData(const QComboBox *widget, const void *context)
{
    auto temp = widget->currentData().value<returnValue>();
    return temp;
}

template<typename returnValue>
inline std::vector<returnValue> readUiData(const QListWidget *widget, const void *context)
{
    QVector<returnValue> data;
    for (int i = 0; i < widget->count(); ++i)
    {
        const auto &entry = widget->item(i);
        const returnValue &format = entry->data(Qt::UserRole).value<returnValue>();
        if (!data.contains(format))
            data += format;
    }
    return data.toStdVector();
}

template<typename Widget, typename Type>
inline void saveUiData(Widget *widget, const Type &value, const void *context = nullptr);

template<>
inline void saveUiData(QCheckBox *widget, const bool &value, const void *context)
{
    widget->setChecked(value);
}

template<>
inline void saveUiData(QRadioButton *widget, const bool &value, const void *context)
{
    widget->setChecked(value);
}

template<>
inline void saveUiData(QAction *widget, const bool &value, const void *context)
{
    widget->setChecked(value);
}

template<>
inline void saveUiData(QGroupBox *widget, const bool &value, const void *context)
{
    widget->setChecked(value);
}

template<>
inline void saveUiData(QGroupBox *widget, const int &value, const void *context)
{
    auto widgetList = widget->findChildren<QRadioButton *>();
    int counter = 0;
    widget->setChecked(false);
    for (auto child : widgetList)
    {
        counter++;
        child->setChecked(false);
        if (counter == value)
        {
            child->setChecked(true);
            widget->setChecked(true);
        }
    }
}

template<>
inline void saveUiData(QWidget *widget, const bool &value, const void *context)
{
    widget->setEnabled(value);
}

template<>
inline void saveUiData(QSpinBox *widget, const int &value, const void *context)
{
    widget->setValue(value);
}

template<>
inline void saveUiData(QDoubleSpinBox *widget, const double &value, const void *context)
{
    double div = context ? *static_cast<const double *>(context) : 1.0;
    widget->setValue(value / div);
}

template<>
inline void saveUiData(QLineEdit *widget, const std::string &value, const void *context)
{
    widget->setText(QString::fromStdString(value));
}

template<typename Type>
inline void saveUiData(QComboBox *widget, const Type &value, const void *context)
{
    for (int i = 0; i < widget->count(); ++i)
    {
        auto lh = widget->itemData(i).value<Type>();
        auto rh = value;
        if (widget->itemData(i).value<Type>() == value)
        {
            widget->setCurrentIndex(i);
            return;
        }
    }
}

template<typename Type>
inline void saveUiData(QListWidget *widget, const std::vector<Type> &value, const void *context)
{
    widget->clear();
    for (const auto &subValue : value)
        widget->addItem(subValue);
}

template<>
inline void saveUiData(QListWidget *widget, const std::vector<DXGI_FORMAT> &value, const void *context)
{
    widget->clear();
    for (const DXGI_FORMAT &format : value)
    {
        auto item = new QListWidgetItem(dxgiFormatToString(format));
        item->setData(Qt::UserRole, format);
        widget->addItem(item);
    }
}

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
        : jsonKey(key)
        , _readMW(readMW)
        , _saveMW(saveMW)
        , _targetUI(TargetUI::mainUi)
    {
    }

    Setting(const QString &key, uiReadBSAFiles readBSA, uiSaveBSAFiles saveBSA)
        : jsonKey(key)
        , _readBSA(readBSA)
        , _saveBSA(saveBSA)
        , _targetUI(TargetUI::bsaUi)

    {
    }

    QString jsonKey;

    void readFromUI(const Ui::MainWindow &ui, const Ui::BSAFilesToPack &bsUi, JSON &json) const
    {
        switch (_targetUI)
        {
            case TargetUI::bsaUi: _readBSA(bsUi, json);
            case TargetUI::mainUi: _readMW(ui, json);
        }
    }
    void saveToUI(Ui::MainWindow &ui, Ui::BSAFilesToPack &bsUi, const JSON &json) const
    {
        switch (_targetUI)
        {
            case TargetUI::bsaUi: _saveBSA(bsUi, json);
            case TargetUI::mainUi: _saveMW(ui, json);
        }
    }
};

// Used to register settings into a list
struct settingBuilder
{
    settingBuilder(QVector<const Setting *> &vec, const Setting *setting) { vec << setting; }
};

#define REGISTER_SETTING_CUSTOM(name, key, readFunc, saveFunc) \
    static const Setting name(key, readFunc, saveFunc); \
    static settingBuilder builder_##name(settingsList, &name);

#define REGISTER_SETTING_WITH_CONTEXT(name, key, uiElement, type, context) \
    REGISTER_SETTING_CUSTOM( \
        name, \
        key, \
        [](const Ui::MainWindow &ui, JSON &json) { json.setValue(key, readUiData<type>(uiElement, context)); }, \
        [](Ui::MainWindow &ui, const JSON &json) { saveUiData(uiElement, json.getValue<type>(key), context); })

#define REGISTER_SETTING(name, key, uiElement, type) REGISTER_SETTING_WITH_CONTEXT(name, key, uiElement, type, nullptr)

#define REGISTER_SETTING_VECTOR(name, key, uiElement, type, context) \
    REGISTER_SETTING_CUSTOM( \
        name, \
        key, \
        [](const Ui::MainWindow &ui, JSON &json) { json.setValue(key, readUiData<type>(uiElement, context)); }, \
        [](Ui::MainWindow &ui, const JSON &json) { \
            saveUiData(uiElement, json.getValue<std::vector<type>>(key), context); \
        })

#define REGISTER_SETTING_CHECKBOX(name, key, uiElement) REGISTER_SETTING(name, key, uiElement, bool)

#define REGISTER_SETTING_NO_UI(name, key) \
    static const Setting name( \
        key, \
        [](const Ui::MainWindow &ui, JSON &json) { return; }, \
        [](Ui::MainWindow &ui, const JSON &json) { return; });

// clang-format off
      static QVector<const Setting*> settingsList;

      REGISTER_SETTING_CHECKBOX(bDebugLog,
          "General/bDebugLog",
          ui.actionEnable_debug_log)

      REGISTER_SETTING_CHECKBOX(bDryRun,
          "General/bDryRun",
          ui.dryRunCheckBox)

      REGISTER_SETTING(eMode,
          "General/eMode",
          ui.modeChooserComboBox,
          OptimizationMode)

      REGISTER_SETTING(sUserPath,
          "General/sUserPath",
          ui.userPathTextEdit,
          std::string)

      REGISTER_SETTING_CHECKBOX(bBsaExtract,
          "BSA/bBsaExtract",
          ui.bsaExtractCheckBox)

      REGISTER_SETTING_CHECKBOX(bBsaCreate,
          "BSA/bBsaCreate",
          ui.bsaCreateCheckbox)

      REGISTER_SETTING_CHECKBOX(bBsaDeleteBackup,
          "BSA/bBsaDeleteBackup",
          ui.bsaDeleteBackupsCheckBox)

      REGISTER_SETTING_CHECKBOX(bBsaLeastBsaPossible,
          "BSA/bBsaLeastBsaPossible",
          ui.bsaLeastBsasCheckBox)

      REGISTER_SETTING_CHECKBOX(bBsaCreateDummies,
          "BSA/bBsaCreateDummies",
          ui.bsaCreateDummiesCheckBox)

      REGISTER_SETTING_CHECKBOX(bTexturesNecessary,
          "Textures/bTexturesNecessary",
          ui.texturesNecessaryOptimizationCheckBox)

      REGISTER_SETTING_CHECKBOX(bTexturesCompress,
          "Textures/bTexturesCompress",
          ui.texturesCompressCheckBox)

      REGISTER_SETTING_CHECKBOX(bTexturesMipmaps,
          "Textures/Textures/Mipmaps",
          ui.texturesMipmapCheckBox)

      REGISTER_SETTING_WITH_CONTEXT(bTexturesResizeSize,
          "Textures/Resizing/BySize/Enabled",
          ui.texturesResizingBySizeRadioButton,
          bool,
          ui.texturesResizingGroupBox)

      REGISTER_SETTING(iTexturesTargetHeight,
          "Textures/Resizing/BySize/Height",
          ui.texturesResizingBySizeHeight,
          int)

      REGISTER_SETTING(iTexturesTargetWidth,
          "Textures/Resizing/BySize/Width",
          ui.texturesResizingBySizeWidth,
          int)

      REGISTER_SETTING_WITH_CONTEXT(bTexturesResizeRatio,
          "Textures/Resizing/ByRatio/Enabled",
          ui.texturesResizingByRatioRadioButton,
          bool,
          ui.texturesResizingGroupBox)

      REGISTER_SETTING(iTexturesTargetWidthRatio,
          "Textures/Resizing/ByRatio/Width",
          ui.texturesResizingByRatioWidth,
          int)

      REGISTER_SETTING(iTexturesTargetHeightRatio,
          "Textures/Resizing/ByRatio/Height",
          ui.texturesResizingByRatioHeight,
          int)

      REGISTER_SETTING(iMeshesOptimizationLevel, 
          "Meshes/iMeshesOptimizationLevel",
          ui.meshesGroupBox,
          int)

      REGISTER_SETTING_CHECKBOX(bAnimationsOptimization,
          "Animations/bAnimationsOptimization",
          ui.animationsNecessaryOptimizationCheckBox)

      REGISTER_SETTING_CHECKBOX(bMeshesHeadparts,
          "Meshes/bMeshesHeadparts",
          ui.meshesHeadpartsCheckBox)

      REGISTER_SETTING_CHECKBOX(bMeshesResave,
          "Meshes/bMeshesResave",
          ui.meshesResaveCheckBox)

      REGISTER_SETTING(bBSATabEnabled,
          "Advanced/BSA/bBSATabEnabled",
          ui.bsaTab,
          bool)

      REGISTER_SETTING(sBSAExtension,
          "Advanced/BSA/sBSAExtension",
          ui.bsaExtension,
          std::string)

      REGISTER_SETTING(eBSAFormat,
          "Advanced/BSA/eBSAFormat",
          ui.bsaFormat,
          bsa_archive_type_e)

      REGISTER_SETTING_WITH_CONTEXT(iBSAMaxSize,
          "Advanced/BSA/iBSAMaxSize",
          ui.bsaMaximumSize,
          double,
          &::GigaByte)

      REGISTER_SETTING(sBSASuffix,
          "Advanced/BSA/sBSASuffix",
          ui.bsaSuffix,
          std::string)

      REGISTER_SETTING(bBSATexturesEnabled,
          "Advanced/BSA/bBSATexturesEnabled",
          ui.bsaTexturesAdvancedGroupBox,
          bool)

      REGISTER_SETTING(eBSATexturesFormat,
          "Advanced/BSA/eBSATexturesFormat",
          ui.bsaTexturesFormat,
          bsa_archive_type_e)

      REGISTER_SETTING_WITH_CONTEXT(iBSATexturesMaxSize,
          "Advanced/BSA/iBSATexturesMaxSize",
          ui.bsaTexturesMaximumSize,
          double,
          &::GigaByte)

      REGISTER_SETTING(sBSATexturesSuffix,
          "Advanced/BSA/sBSATexturesSuffix",
          ui.bsaTexturesSuffix,
          std::string)

      REGISTER_SETTING(bTexturesTabEnabled,
          "Advanced/Textures/bTexturesTabEnabled",
          ui.texturesTab,
          bool)

      REGISTER_SETTING(eTexturesFormat,
          "Advanced/Textures/eTexturesFormat",
          ui.texturesOutputFormat,
          DXGI_FORMAT)

      REGISTER_SETTING_CHECKBOX(bTexturesTGAConvertEnabled,
          "Advanced/Textures/bTexturesTGAConvertEnabled",
          ui.texturesTgaConversionCheckBox)

      REGISTER_SETTING_CHECKBOX(bTexturesInterfaceConvert,
          "Advanced/Textures/bTexturesInterfaceConvert",
          ui.texturesCompressInterfaceCheckBox)

      REGISTER_SETTING_VECTOR(slTexturesUnwantedFormats,
          "Advanced/Textures/slTexturesUnwantedFormats",
          ui.texturesUnwantedFormatsList,
          DXGI_FORMAT,
          nullptr)

      REGISTER_SETTING(bMeshesTabEnabled,
          "Advanced/Meshes/bMeshesTabEnabled",
          ui.meshesTab,
          bool)

      REGISTER_SETTING(eMeshesFileVersion,
          "Advanced/Meshes/eMeshesFileVersion",
          ui.meshesVersion,
          NiFileVersion)

      REGISTER_SETTING(iMeshesStream,
          "Advanced/Meshes/iMeshesStream",
          ui.meshesStream,
          int)

      REGISTER_SETTING(iMeshesUser,
          "Advanced/Meshes/iMeshesUser",
          ui.meshesUser,
          int)

      REGISTER_SETTING(bAnimationsTabEnabled,
          "Advanced/Animations/bAnimationsTabEnabled",
          ui.AnimationsTab,
          bool)

      REGISTER_SETTING_NO_UI(eAnimationsFormat,
          "Advanced/Animations/eAnimationsFormat")

// clang-format on
#undef REGISTER_SETTING
#undef REGISTER_SETTING_NO_UI
#undef REGISTER_SETTING_CHECKBOX

      } // namespace CAO
