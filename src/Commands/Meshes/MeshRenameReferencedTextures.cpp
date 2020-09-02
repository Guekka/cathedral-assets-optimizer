/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "MeshRenameReferencedTextures.hpp"
#include "Settings/Games.hpp"
#include "Utils/Algorithms.hpp"

namespace CAO {
CommandResult MeshRenameReferencedTextures::process(File &file) const
{
    auto nif = dynamic_cast<MeshResource *>(&file.getFile(true));
    if (!nif)
        return _resultFactory.getCannotCastFileResult();

    for (NiShader *shader : getShaders(*nif))
    {
        std::string tex;
        int texCounter = 0;

        while (nif->GetTextureSlot(shader, tex, texCounter) && !tex.empty())
        {
            if (ends_with(std::string_view(tex), std::string_view(".tga"), false))
            {
                replace_all(tex, std::string_view(".tga"), std::string_view(".dds"), false);
                nif->SetTextureSlot(shader, tex, texCounter);
            }
            else if (++texCounter > limit)
            {
                return _resultFactory
                    .getFailedResult(-1, "Failed to renamed referenced textures from TGA to DDS in mesh");
            }
        }
    }

    return _resultFactory.getSuccessfulResult();
}

bool MeshRenameReferencedTextures::isApplicable(File &file) const
{
    const auto &game = GameSettings::get(currentProfile().getGeneralSettings().eGame());
    if (!game.cMeshesVersion().has_value())
        return false;

    auto nif = dynamic_cast<MeshResource *>(&file.getFile(false));
    if (!nif)
        return false;

    for (NiShader *shader : getShaders(*nif))
    {
        std::string tex;
        int texCounter = 0;
        while (nif->GetTextureSlot(shader, tex, texCounter) && !tex.empty())
        {
            if (ends_with(std::string_view(tex), std::string_view(".tga"), false))
                return true;
            if (++texCounter > limit)
                return false;
        }
    }
    return false;
}

std::vector<NiShader *> MeshRenameReferencedTextures::getShaders(NifFile &nif) const
{
    return nif.GetShapes() | rx::transform([&nif](auto &&shape) { return nif.GetShader(shape); })
           | rx::filter([](auto &&shader) { return bool(shader); }) | rx::to_vector();
}
} // namespace CAO
