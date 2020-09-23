/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/CommandBook.hpp"

namespace CAO {
class MeshRenameReferencedTextures : public Command
{
public:
    QString name() const override { return "Rename referenced textures in Mesh"; }
    bool isOptimization() const override { return true; }
    CommandType type() const override { return CommandType::Mesh; };
    Priority priority() const override { return Low; };

    CommandResult process(File &file) const override;
    CommandState isApplicable(File &file) const override;

private:
    static constexpr int limit = 1000;
    std::vector<NiShader *> getShaders(NifFile &nif) const;
};
REGISTER_COMMAND(MeshRenameReferencedTextures)
} // namespace CAO
