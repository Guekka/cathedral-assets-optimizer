/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/BSA/Utils/BSA.hpp"
#include "Settings.hpp"
#include "Utils/QJSONValueWrapper.hpp"

#define REGISTER_SETTING(type, name, key) \
    QJSONValueWrapper<type> name{json_, nlohmann::json_pointer<nlohmann::json>{key}};

namespace CAO {
class FileTypes : public Settings
{
public:
    std::optional<QString> isValid() const override;

    FileTypes() = default;
    FileTypes(const FileTypes &other);
    FileTypes(FileTypes &&other) noexcept;

    FileTypes &operator=(const FileTypes &other);
    FileTypes &operator=(FileTypes &&other) noexcept;

    bool match(const std::vector<std::string> &patterns, const QString &str) const;

    REGISTER_SETTING(std::vector<std::string>, slBSAStandardFiles, "/BSA/slStandardFiles")
    REGISTER_SETTING(std::vector<std::string>, slBSATextureFiles, "/BSA/slTextureFiles")
    REGISTER_SETTING(std::vector<std::string>, slBSAUncompressibleFiles, "/BSA/slUncompressibleFiles")
    REGISTER_SETTING(std::vector<std::string>, slBSABlacklist, "/BSA/slBlacklist")

    REGISTER_SETTING(std::vector<std::string>, slMeshesHeadparts, "/Meshes/slHeadparts")

    REGISTER_SETTING(std::vector<std::string>, slTextureLandscapes, "/Textures/slLandscape")

    REGISTER_SETTING(std::vector<std::string>, slModsBlacklist, "/Mods/slBlacklist")

}; // namespace CAO
} // namespace CAO

#undef REGISTER_SETTING
