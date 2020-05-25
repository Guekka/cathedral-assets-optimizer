/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/BSA/Utils/BSA.hpp"
#include "Settings.hpp"

#define REGISTER_SETTING(type, name, key) QJSONValueWrapper<type> name{json_, key};

namespace CAO {
class FileTypes : public Settings
{
public:
    std::optional<QString> isValid() const override;

    FileTypes() {}
    FileTypes([[maybe_unused]] const FileTypes &other) {}

    REGISTER_SETTING(std::vector<std::string>, slBSAStandardFiles, "BSA/slBSAStandardFiles")
    REGISTER_SETTING(std::vector<std::string>, slBSATextureFiles, "BSA/slBSATextureFiles")
    REGISTER_SETTING(std::vector<std::string>, slBSAUncompressibleFiles, "BSA/slBSAUncompressibleFiles")

    REGISTER_SETTING(std::vector<std::string>, slTextureFiles, "Textures/slTextureFiles")

    REGISTER_SETTING(std::vector<std::string>, slMeshFiles, "Meshes/slMeshFiles")
    REGISTER_SETTING(std::vector<std::string>, slHeadparts, "Meshes/slHeadparts")

    REGISTER_SETTING(std::vector<std::string>, slAnimationFiles, "Animations/slAnimationFiles")
};
} // namespace CAO

#undef REGISTER_SETTING
