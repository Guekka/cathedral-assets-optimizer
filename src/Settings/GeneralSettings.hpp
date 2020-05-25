/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings.hpp"

#define REGISTER_SETTING(type, name, key) QJSONValueWrapper<type> name{json_, key};

namespace CAO {
class GeneralSettings final : public Settings
{ //TODO: V690 http://www.viva64.com/en/V690 The 'GeneralSettings' class implements a move constructor, but lacks the move assignment operator. It is dangerous to use such a class.
public:
    GeneralSettings() = default;
    GeneralSettings(nlohmann::json j);
    GeneralSettings(const GeneralSettings &other);
    GeneralSettings(GeneralSettings &&other);

    void operator=(const GeneralSettings &other);

    std::optional<QString> isValid() const override;

    //All the code below is used to register settings

    REGISTER_SETTING(bool, bBSACreate, "BSA/bBSACreate")
    REGISTER_SETTING(bool, bBSAExtractFromBSA, "BSA/bBSAExtractFromBSA")
    REGISTER_SETTING(bool, bBSACreateDummies, "BSA/bBSACreateDummies")
    REGISTER_SETTING(bool, bBSACompressArchive, "BSA/bBSACompressArchive")

    REGISTER_SETTING(bsa_archive_type_t, eBSAFormat, "Advanced/BSA/eBSAFormat")
    REGISTER_SETTING(bsa_archive_type_t, eBSATexturesFormat, "Advanced/BSA/eBSATexturesFormat")
    REGISTER_SETTING(double, iBSAMaxSize, "Advanced/BSA/iBSAMaxSize")
    REGISTER_SETTING(bool, bBSATexturesEnabled, "Advanced/BSA/bBSATexturesEnabled")
    REGISTER_SETTING(double, iBSATexturesMaxSize, "Advanced/BSA/iBSATexturesMaxSize")

    REGISTER_SETTING(bool, bDebugLog, "General/bDebugLog")
    REGISTER_SETTING(bool, bDryRun, "General/bDryRun")
    REGISTER_SETTING(OptimizationMode, eMode, "General/eMode")
    REGISTER_SETTING(QString, sUserPath, "General/sUserPath")

    REGISTER_SETTING(bool, bBSACompact, "BSA/bBSACompact")
    REGISTER_SETTING(bool, bBSADeleteBackup, "BSA/bBSADeleteBackup")
    REGISTER_SETTING(QString, sBSAExtension, "Advanced/BSA/sBSAExtension")
    REGISTER_SETTING(QString, sBSASuffix, "Advanced/BSA/sBSASuffix")
    REGISTER_SETTING(QString, sBSATexturesSuffix, "Advanced/BSA/sBSATexturesSuffix")

    REGISTER_SETTING(bool, bBSATabEnabled, "Advanced/BSA/bBSATabEnabled")
    REGISTER_SETTING(bool, bTexturesTabEnabled, "Advanced/Textures/bTexturesTabEnabled")
    REGISTER_SETTING(bool, bMeshesTabEnabled, "Advanced/Meshes/bMeshesTabEnabled")
    REGISTER_SETTING(bool, bAnimationsTabEnabled, "Advanced/Animations/bAnimationsTabEnabled")
};
} // namespace CAO

#undef REGISTER_SETTING
