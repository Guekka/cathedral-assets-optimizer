/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/BSA/Utils/BSA.hpp"
#include "Settings.hpp"

#define REGISTER_SETTING(type, name, key) \
    QJSONValueWrapper<type> name{json_, nlohmann::json_pointer<nlohmann::json>{key}};

namespace CAO {
class FileTypes : public Settings
{
public:
    std::optional<QString> isValid() const override;

    FileTypes() = default;
    FileTypes(const FileTypes &other);
    FileTypes(FileTypes &&other);

    FileTypes &operator=(const FileTypes &other);
    FileTypes &operator=(FileTypes &&other);

    REGISTER_SETTING(std::vector<std::string>, slBSAStandardFiles, "/BSA/slBSAStandardFiles")
    REGISTER_SETTING(std::vector<std::string>, slBSATextureFiles, "/BSA/slBSATextureFiles")
    REGISTER_SETTING(std::vector<std::string>, slBSAUncompressibleFiles, "/BSA/slBSAUncompressibleFiles")

    REGISTER_SETTING(std::vector<std::string>, slMeshesHeadparts, "/Meshes/slHeadparts")

    REGISTER_SETTING(std::vector<std::string>, slTextureLandscapes, "/Textures/slLandscape")
}; // namespace CAO
} // namespace CAO

#undef REGISTER_SETTING
