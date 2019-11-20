/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "GUI/ui_mainWindow.h"
#include "pch.hpp"

constexpr double GigaByte = static_cast<double>(1024 * 1024 * 1024);

Q_DECLARE_METATYPE(DXGI_FORMAT)
Q_DECLARE_METATYPE(NiFileVersion)
Q_DECLARE_METATYPE(bsa_archive_type_e)

namespace CAO {
namespace AdvancedSettings {
enum AdvancedKey
{
    bBSATabEnabled,
    sBSAExtension,

    eBSAFormat,
    iBSAMaxSize,
    sBSASuffix,

    bBSATexturesEnabled,
    eBSATexturesFormat,
    iBSATexturesMaxSize,
    sBSATexturesSuffix,

    bTexturesTabEnabled,
    eTexturesFormat,
    bTexturesTGAConvertEnabled,
    bTexturesInterfaceConvert,
    slTexturesUnwantedFormats,

    bMeshesTabEnabled,
    eMeshesFileVersion,
    iMeshesStream,
    iMeshesUser,

    bAnimationsTabEnabled,
    eAnimationsFormat,

};
const nlohmann::json &toJSONKey(const AdvancedKey &key, const nlohmann::json &json);
nlohmann::json &toJSONKey(const AdvancedKey &key, nlohmann::json &json);

#ifdef GUI
void saveToUi(Ui::MainWindow *ui, const nlohmann::json &json);
void readFromUi(Ui::MainWindow *ui, nlohmann::json &json);
#endif
}; // namespace AdvancedSettings
} // namespace CAO
