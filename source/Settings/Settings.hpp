
/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "GUI/MainWindow.hpp"
#include "UISync.hpp"
#include "Utils/QValueWrapper.hpp"
#include "pch.hpp"
#include "ui_BSAFilesToPack.h"

namespace CAO {

#define REGISTER_SETTING(type, name, key) QJSONValueWrapper<type> name{key, json_};

class Settings
{
public:
    /* TODO
    //! \brief Checks if the current settings are allowed 
    virtual std::optional<QString> isValid() = 0; 
    */
    Settings() = default;
    Settings(const nlohmann::json &j)
        : json_(j)
    {}

    virtual ~Settings() = default;

    virtual nlohmann::json getJSON() const { return json_; }
    virtual void setJSON(const nlohmann::json &j) { json_ = j; }

    void saveToUi(MainWindow &window) const;
    void readFromUi(const MainWindow &window);

protected:
    std::vector<UISync> uiSyncList_;
    mutable nlohmann::json json_;
};
class PatternSettings final : public Settings
{
public:
    PatternSettings(const nlohmann::json &json);
    PatternSettings(size_t priority, const std::vector<QRegularExpression> &regex);

    std::vector<QRegularExpression> regexes_;
    size_t priority_{0};

    nlohmann::json getJSON() const override;
    void setJSON(const nlohmann::json &j) override;

    std::optional<QString> isValid();

    static constexpr auto patternKey = "Pattern";
    static constexpr auto regexKey = "Regex";
    static constexpr auto priorityKey = "Priority";

private:
    std::vector<QRegularExpression> getPatternRegexFromJSON(const nlohmann::json &json);
    std::optional<size_t> getPatternPriorityFromJSON(const nlohmann::json &json);

public:
    REGISTER_SETTING(bool, bBSAExtractFromBSA, "BSA/bBSAExtractFromBSA")
    REGISTER_SETTING(bool, bBSAAddToBSA, "BSA/bBSAAddToBSA")
    REGISTER_SETTING(bool, bBSACreateDummies, "BSA/bBSACreateDummies")
    REGISTER_SETTING(bool, bTexturesNecessary, "Textures/bTexturesNecessary")
    REGISTER_SETTING(bool, bTexturesCompress, "Textures/bTexturesCompress")
    REGISTER_SETTING(bool, bTexturesMipmaps, "Textures/bTexturesMipmaps")
    REGISTER_SETTING(bool, bTexturesResizeSize, "Textures/Resizing/BySize/Enabled")
    REGISTER_SETTING(int, iTexturesTargetHeight, "Textures/Resizing/BySize/Height")
    REGISTER_SETTING(int, iTexturesTargetWidth, "Textures/Resizing/BySize/Width")
    REGISTER_SETTING(bool, bTexturesResizeRatio, "Textures/Resizing/ByRatio/Enabled")
    REGISTER_SETTING(int, iTexturesTargetWidthRatio, "Textures/Resizing/ByRatio/Width")
    REGISTER_SETTING(int, iTexturesTargetHeightRatio, "Textures/Resizing/ByRatio/Height")
    REGISTER_SETTING(int, iMeshesOptimizationLevel, "Meshes/iMeshesOptimizationLevel")
    REGISTER_SETTING(bool, bAnimationsOptimization, "Animations/bAnimationsOptimization")
    REGISTER_SETTING(bool, bMeshesHeadparts, "Meshes/bMeshesHeadparts")
    REGISTER_SETTING(bool, bMeshesResave, "Meshes/bMeshesResave")
    REGISTER_SETTING(DXGI_FORMAT, eTexturesFormat, "Advanced/Textures/eTexturesFormat")
    REGISTER_SETTING(bsa_archive_type_t, eBSAFormat, "Advanced/BSA/eBSAFormat")
    REGISTER_SETTING(double, iBSAMaxSize, "Advanced/BSA/iBSAMaxSize")
    REGISTER_SETTING(bool, bBSATexturesEnabled, "Advanced/BSA/bBSATexturesEnabled")
    REGISTER_SETTING(bsa_archive_type_t, eBSATexturesFormat, "Advanced/BSA/eBSATexturesFormat")
    REGISTER_SETTING(double, iBSATexturesMaxSize, "Advanced/BSA/iBSATexturesMaxSize")
    REGISTER_SETTING(bool, bBSAIsStandard, "Advanced/BSA/bBSAIsStandard")
    REGISTER_SETTING(bool, bBSAIsTexture, "Advanced/BSAbBSAIsTexture")
    REGISTER_SETTING(bool, bBSAIsUncompressible, "Advanced/BSA/bBSAIsUncompressible")
    REGISTER_SETTING(bool, bTexturesForceConvert, "Advanced/Textures/bTexturesForceConvert")
    REGISTER_SETTING(NiFileVersion, eMeshesFileVersion, "Advanced/Meshes/eMeshesFileVersion")
    REGISTER_SETTING(uint, iMeshesStream, "Advanced/Meshes/iMeshesStream")
    REGISTER_SETTING(uint, iMeshesUser, "Advanced/Meshes/iMeshesUser")
    REGISTER_SETTING(hkPackFormat, eAnimationsFormat, "Advanced/Animations/eAnimationsFormat")
};

class GeneralSettings final : public Settings
{
    Q_GADGET
public:
    GeneralSettings() = default;
    GeneralSettings(nlohmann::json j);

    /*!
   * \brief Checks if the current settings are allowed
   */
    QString isValid() const;

public:
    //All the code below is used to register settings

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
