/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/Animations/Animation.hpp"
#include "Commands/BSA/BSA.hpp"
#include "Commands/Meshes/Mesh.hpp"
#include "Commands/Textures/Texture.hpp"
#include "Settings/Settings.hpp"

namespace CAO {
/*!
 * \brief Coordinates all the subclasses in order to optimize BSAs, textures, meshes and animations
 */
class MainOptimizer final : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(MainOptimizer)

public:
    explicit MainOptimizer();

    struct MemoryData
    {
        void *pSource               = nullptr;
        size_t sourceSize           = 0;
        std::vector<std::byte> *out = nullptr;
    };

    void process(File &file, bool dryRun = false, MemoryData memoryData = {nullptr, 0, nullptr});
    void packBsa(const QString &folder);

private:
    bool runCommand(const Command &command, File &file);
    bool dryRunCommand(const Command &command, File &file);

    bool processReal(File &file, std::vector<std::byte> *out = nullptr);
    bool processDry(File &file);
    bool processBSA(File &file, bool dryRun);

    bool loadFile(File &file, void *pSource = nullptr, size_t size = 0);
    bool saveFile(File &file, std::vector<std::byte> *out = nullptr);


    CommandBook _commandBook;
};
} // namespace CAO
