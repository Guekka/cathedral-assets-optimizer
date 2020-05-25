/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings.hpp"

#define REGISTER_SETTING(type, name, key) QJSONValueWrapper<type> name{json_, key};

namespace CAO {
class PatternSettings final : public Settings
{
public:
    PatternSettings();
    PatternSettings(const nlohmann::json &json);
    PatternSettings(size_t priority, const std::vector<std::string> &regex);
    PatternSettings(const PatternSettings &other);
    PatternSettings(PatternSettings &&other);

    void operator=(const PatternSettings &other);
    void operator=(PatternSettings &&other);
    bool operator==(const PatternSettings &other) const;

    std::optional<QString> isValid() const override;

    std::vector<std::string> patterns_;
    size_t priority_{0};

    void setJSON(const nlohmann::json &j) override;
    nlohmann::json getJSON() const override;
    static nlohmann::json removeMeta(const nlohmann::json &j);

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
} // namespace CAO

namespace std {
template<>
struct hash<CAO::PatternSettings>
{
    size_t operator()(const CAO::PatternSettings &x) const
    {
        return std::hash<nlohmann::json>{}(x.getJSON());
    }
};
} // namespace std

#undef REGISTER_SETTING
