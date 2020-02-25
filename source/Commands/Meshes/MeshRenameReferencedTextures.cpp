/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "MeshRenameReferencedTextures.hpp"

namespace CAO {
CommandResult MeshRenameReferencedTextures::process(File& file)
{
    auto nif = dynamic_cast<MeshResource *>(&file.getFile(true));
    if (!nif)
        return _resultFactory.getCannotCastFileResult();

    bool meshChanged = false;
    constexpr int limit = 1000;

    for (auto shape : nif->GetShapes())
    {
        auto shader = nif->GetShader(shape);
        if (shader)
        {
            std::string tex;
            int texCounter = 0;
            while (nif->GetTextureSlot(shader, tex, texCounter) && !tex.empty())
            {
                QString qsTex = QString::fromStdString(tex);
                if (qsTex.contains(".tga", Qt::CaseInsensitive))
                {
                    qsTex.replace(".tga", ".dds", Qt::CaseInsensitive);
                    tex = qsTex.toStdString();
                    nif->SetTextureSlot(shader, tex, texCounter);
                    meshChanged = true;
                }
                if (++texCounter > limit)
                {
                    return _resultFactory
                        .getFailedResult(1, "Failed to renamed referenced textures from TGA to DDS in mesh");
                }
            }
        }
    }
    file.setOptimizedCurrentFile(meshChanged);

    auto result = _resultFactory.getSuccessfulResult();
    result.processedFile = meshChanged;
    return result;
}

bool MeshRenameReferencedTextures::isApplicable(File& file)
{
    bool isMeshFile = dynamic_cast<const MeshResource *>(&file.getFile());
    return isMeshFile;
}
} // namespace CAO