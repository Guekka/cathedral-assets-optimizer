/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/CommandBook.hpp"
#include "Commands/Plugins/PluginsOperations.hpp"
#include "File/Meshes/MeshFile.hpp"
#include "Utils/Filesystem.hpp"

namespace CAO {
class MeshConvert : public Command
{
public:
    QString name() override { return "Convert Mesh"; }
    CommandType type() override { return CommandType::Mesh; };
    Priority priority() override { return Priority::Medium; };

    CommandResult process(File& file) override;
    bool isApplicable(File& file) override;

protected:
    bool isHeadpart(const QString &filepath);
    void listHeadparts(const GeneralSettings &settings, FileTypes &filetypes);

    std::once_flag _onceHeadpartsFlag;
};

} // namespace CAO
