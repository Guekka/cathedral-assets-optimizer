/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/CommandBook.hpp"
#include "Commands/Plugins/PluginsOperations.hpp"
#include "File/Meshes/MeshFile.hpp"
#include "Utils/CallOnce.hpp"

namespace CAO {
class MeshConvert : public Command
{
public:
    MeshConvert();

    QString name() const override { return "Convert Mesh"; }
    CommandType type() const override { return CommandType::Mesh; };
    Priority priority() const override { return Priority::Medium; };

    CommandResult process(File& file) const override;
    bool isApplicable(File& file) const override;

    static bool isHeadpart(const QString &filepath);
    static void listHeadparts(const GeneralSettings &settings, FileTypes &filetypes);

    static inline CallOnce callOnceHeadparts_;
};
REGISTER_COMMAND(MeshConvert)
} // namespace CAO
