/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "BaseTypes.hpp"
#include "Settings.hpp"

#define REGISTER_SETTING(type, name, key) \
    QJSONValueWrapper<type> name{json_, nlohmann::json_pointer<nlohmann::json>{key}};

namespace CAO {
class GeneralSettings final : public Settings
{
public:
    GeneralSettings() = default;
    GeneralSettings(nlohmann::json j);
    GeneralSettings(const GeneralSettings &other);
    GeneralSettings(GeneralSettings &&other) noexcept;

    GeneralSettings &operator=(const GeneralSettings &other);
    GeneralSettings &operator=(GeneralSettings &&other) noexcept;

    std::optional<QString> isValid() const override;

private:
    static inline const QString forceProcessFolder = "ForceProcess.cao";

    enum class ModManager
    {
        Vortex,
        MO2,
        Kortex,
        ManualForced,
        None
    };
    ModManager findManager(const QDir &dir) const;

public:
    REGISTER_SETTING(bool, bBSACreate, "/BSA/bBSACreate")
    REGISTER_SETTING(bool, bBSAExtractFromBSA, "/BSA/bBSAExtractFromBSA")
    REGISTER_SETTING(bool, bBSACreateDummies, "/BSA/bBSACreateDummies")
    REGISTER_SETTING(bool, bBSACompressArchive, "/BSA/bBSACompressArchive")
    REGISTER_SETTING(bool, bBSAProcessContent, "/BSA/bBSAProcessContent")

    REGISTER_SETTING(double, iBSAMaxSize, "/Advanced/BSA/iBSAMaxSize")
    REGISTER_SETTING(bool, bBSATexturesEnabled, "/Advanced/BSA/bBSATexturesEnabled")
    REGISTER_SETTING(double, iBSATexturesMaxSize, "/Advanced/BSA/iBSATexturesMaxSize")

    REGISTER_SETTING(bool, isBaseProfile, "/General/isBaseProfile")

    REGISTER_SETTING(bool, bDryRun, "/General/bDryRun")
    REGISTER_SETTING(OptimizationMode, eMode, "/General/eMode")
    REGISTER_SETTING(Games, eGame, "/General/Game")

    REGISTER_SETTING(bool, bEnableOutputPath, "/General/bEnableOutputPath")
    REGISTER_SETTING(QString, sInputPath, "/General/sInputPath")
    REGISTER_SETTING(QString, sOutputPath, "/General/sOutputPath")

    REGISTER_SETTING(bool, bBSACompact, "/BSA/bBSACompact")
    REGISTER_SETTING(bool, bBSADeleteBackup, "/BSA/bBSADeleteBackup")
};
} // namespace CAO

#undef REGISTER_SETTING
