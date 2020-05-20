
/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "GUI/MainWindow.hpp"
#include "UISync.hpp"
#include "Utils/QJSONValueWrapper.hpp"
#include "pch.hpp"

namespace CAO {

#define REGISTER_SETTING(type, name, key) QJSONValueWrapper<type> name{json_, key};

class Settings //TODO: V690 http://www.viva64.com/en/V690 The 'Settings' class implements a move constructor, but lacks the move assignment operator. It is dangerous to use such a class.
{
public:
    /* TODO
    //! \brief Checks if the current settings are allowed 
    virtual std::optional<QString> isValid() = 0; 
    */

    Settings() = default;
    Settings(const nlohmann::json &j);
    Settings(const Settings &other);
    Settings(Settings &&other);
    virtual ~Settings() = default;

    void operator=(const Settings &other);

    virtual nlohmann::json getJSON() const { return json_; }
    virtual void setJSON(const nlohmann::json &j) { json_ = j; }

    void saveToUi(MainWindow &window) const;
    void readFromUi(const MainWindow &window);

protected:
    std::vector<UISync> uiSyncList_;
    mutable nlohmann::json json_;
};
class PatternSettings final : public Settings //TODO: V690 http://www.viva64.com/en/V690 The 'PatternSettings' class implements a move constructor, but lacks the move assignment operator. It is dangerous to use such a class.
{
public:
    PatternSettings();
    PatternSettings(const nlohmann::json &json);
    PatternSettings(size_t priority, const std::vector<std::string> &regex);
    PatternSettings(const PatternSettings &other);
    PatternSettings(PatternSettings &&other);

    void operator=(const PatternSettings &other);
    bool operator==(const PatternSettings &other) const;

    std::vector<std::string> patterns_;
    size_t priority_{0};

    nlohmann::json getJSON() const override;
    nlohmann::json getJSONWithoutMeta() const;
    void setJSON(const nlohmann::json &j) override;

    std::optional<QString> isValid();

    static constexpr auto patternKey  = "Patterns";
    static constexpr auto priorityKey = "Priority";

private:
    std::vector<std::string> getPatternWildcardsFromJSON(const nlohmann::json &json);
    std::optional<size_t> getPatternPriorityFromJSON(const nlohmann::json &json);

public:
    REGISTER_SETTING(bool, bBSAAddToBSA, "BSA/bBSAAddToBSA")
    REGISTER_SETTING(bool, bBSAIsStandard, "Advanced/BSA/bBSAIsStandard")
    REGISTER_SETTING(bool, bBSAIsTexture, "Advanced/BSA/bBSAIsTexture")
    REGISTER_SETTING(bool, bBSAIsUncompressible, "Advanced/BSA/bBSAIsUncompressible")

    REGISTER_SETTING(bool, bTexturesNecessary, "Textures/bTexturesNecessary")
    REGISTER_SETTING(bool, bTexturesCompress, "Textures/bTexturesCompress")
    REGISTER_SETTING(bool, bTexturesMipmaps, "Textures/bTexturesMipmaps")

    REGISTER_SETTING(bool, bTexturesResizeMinimum, "Textures/Resizing/ByRatio/Minimum/Enabled")
    REGISTER_SETTING(int, iTexturesMinimumWidth, "Textures/Resizing/ByRatio/Minimum/Width")
    REGISTER_SETTING(int, iTexturesMinimumHeight, "Textures/Resizing/ByRatio/Minimum/Height")

    REGISTER_SETTING(TextureResizingMode, eTexturesResizingMode, "Textures/Resizing/Mode")

    REGISTER_SETTING(int, iTexturesResizingBySizeHeight, "Textures/Resizing/BySize/Height")
    REGISTER_SETTING(int, iTexturesResizingBySizeWidth, "Textures/Resizing/BySize/Width")

    REGISTER_SETTING(int, iTexturesResizingByRatioWidth, "Textures/Resizing/ByRatio/Width")
    REGISTER_SETTING(int, iTexturesResizingByRatioHeight, "Textures/Resizing/ByRatio/Height")

    REGISTER_SETTING(DXGI_FORMAT, eTexturesFormat, "Advanced/Textures/eTexturesFormat")
    REGISTER_SETTING(bool, bTexturesForceConvert, "Advanced/Textures/bTexturesForceConvert")
    REGISTER_SETTING(std::vector<DXGI_FORMAT>,
                     slTextureUnwantedFormats,
                     "Advanced/Textures/slTextureUnwantedFormats")

    REGISTER_SETTING(int, iMeshesOptimizationLevel, "Meshes/iMeshesOptimizationLevel")
    REGISTER_SETTING(bool, bMeshesHeadparts, "Meshes/bMeshesHeadparts")
    REGISTER_SETTING(bool, bMeshesResave, "Meshes/bMeshesResave")
    REGISTER_SETTING(NiFileVersion, eMeshesFileVersion, "Advanced/Meshes/eMeshesFileVersion")
    REGISTER_SETTING(uint, iMeshesStream, "Advanced/Meshes/iMeshesStream")
    REGISTER_SETTING(uint, iMeshesUser, "Advanced/Meshes/iMeshesUser")

    REGISTER_SETTING(bool, bAnimationsOptimization, "Animations/bAnimationsOptimization")
    REGISTER_SETTING(hkPackFormat, eAnimationsFormat, "Advanced/Animations/eAnimationsFormat")
};

class GeneralSettings final : public Settings
{ //TODO: V690 http://www.viva64.com/en/V690 The 'GeneralSettings' class implements a move constructor, but lacks the move assignment operator. It is dangerous to use such a class.
public:
    GeneralSettings() = default;
    GeneralSettings(nlohmann::json j);
    GeneralSettings(const GeneralSettings &other);
    GeneralSettings(GeneralSettings &&other);

    void operator=(const GeneralSettings &other);

    //! \brief Checks if the current settings are allowed
    QString isValid() const;

    //All the code below is used to register settings

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
#undef REGISTER_SETTING
} // namespace CAO

namespace std {
template<>
struct hash<CAO::PatternSettings>
{
    size_t operator()(const CAO::PatternSettings &x) const
    {
        auto jHasher = std::hash<nlohmann::json>{};
        return jHasher(x.getJSON());
    }
};
} // namespace std
