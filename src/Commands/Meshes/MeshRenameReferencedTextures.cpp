/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MeshRenameReferencedTextures.hpp"
#include "File/Meshes/MeshFile.hpp"
#include "Settings/Games.hpp"
#include "Utils/Algorithms.hpp"

namespace CAO {
CommandResult MeshRenameReferencedTextures::process(File &file) const
{
    auto *nif = file.getFile<Resources::Mesh>(true);
    if (!nif)
        return CommandResultFactory::getCannotCastFileResult();

    for (auto *shape : nif->GetShapes())
        for (auto tex : nif->GetTexturePathRefs(shape))
            replace_all(tex.get(), std::string_view(".tga"), std::string_view(".dds"), false);

    return CommandResultFactory::getSuccessfulResult();
}

CommandState MeshRenameReferencedTextures::isApplicable(File &file) const
{
    const auto &game = GameSettings::get(currentProfile().getGeneralSettings().eGame());
    if (!game.cMeshesVersion().has_value())
        return CommandState::NotRequired;

    auto *nif = file.getFile<Resources::Mesh>(false);
    if (!nif)
        return CommandState::NotRequired;

    for (auto *shape : nif->GetShapes())
        for (auto tex : nif->GetTexturePathRefs(shape))
            replace_all(tex.get(), std::string_view(".tga"), std::string_view(".dds"), false);

    return CommandState::NotRequired;
}

std::vector<nifly::NiShader *> MeshRenameReferencedTextures::getShaders(nifly::NifFile &nif) const
{
    return nif.GetShapes() | rx::transform([&nif](auto &&shape) { return nif.GetShader(shape); })
           | rx::filter([](auto &&shader) { return bool(shader); }) | rx::to_vector();
}
} // namespace CAO
