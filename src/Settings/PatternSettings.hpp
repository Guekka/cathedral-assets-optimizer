/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "BaseTypes.hpp"
#include "Settings.hpp"
#include "Utils/QJSONValueWrapper.hpp"

#define REGISTER_SETTING(type, name, key) \
    QJSONValueWrapper<type> name{json_, nlohmann::json_pointer<nlohmann::json>{key}};

namespace CAO {
class PatternSettings final : public Settings
{
public:
    PatternSettings();
    PatternSettings(const nlohmann::json &json);
    PatternSettings(size_t priority, const std::vector<std::string> &regex);
    PatternSettings(const PatternSettings &other);
    PatternSettings(PatternSettings &&other) noexcept;

    PatternSettings &operator=(const PatternSettings &other);
    PatternSettings &operator=(PatternSettings &&other) noexcept; 
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
    REGISTER_SETTING(bool, bTexturesNecessary, "/Textures/bTexturesNecessary")
    REGISTER_SETTING(bool, bTexturesCompress, "/Textures/bTexturesCompress")
    REGISTER_SETTING(bool, bTexturesMipmaps, "/Textures/bTexturesMipmaps")
    REGISTER_SETTING(bool, bTexturesLandscapeAlpha, "/Textures/bTexturesLandscapeAlpha")

    REGISTER_SETTING(bool, bTexturesResizeMinimum, "/Textures/Resizing/ByRatio/Minimum/Enabled")
    REGISTER_SETTING(int, iTexturesMinimumWidth, "/Textures/Resizing/ByRatio/Minimum/Width")
    REGISTER_SETTING(int, iTexturesMinimumHeight, "/Textures/Resizing/ByRatio/Minimum/Height")

    REGISTER_SETTING(TextureResizingMode, eTexturesResizingMode, "/Textures/Resizing/Mode")

    REGISTER_SETTING(int, iTexturesResizingHeight, "/Textures/Resizing/Height")
    REGISTER_SETTING(int, iTexturesResizingWidth, "/Textures/Resizing/Width")

    REGISTER_SETTING(DXGI_FORMAT, eTexturesFormat, "/Advanced/Textures/eTexturesFormat")
    REGISTER_SETTING(bool, bTexturesForceConvert, "/Advanced/Textures/bTexturesForceConvert")
    REGISTER_SETTING(std::vector<DXGI_FORMAT>,
                     slTextureUnwantedFormats,
                     "/Advanced/Textures/slTextureUnwantedFormats")

    REGISTER_SETTING(int, iMeshesOptimizationLevel, "/Meshes/iMeshesOptimizationLevel")
    REGISTER_SETTING(bool, bMeshesIgnoreHeadparts, "/Meshes/bMeshesIgnoreHeadparts")

    REGISTER_SETTING(bool, bAnimationsOptimization, "/Animations/bAnimationsOptimization")
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
