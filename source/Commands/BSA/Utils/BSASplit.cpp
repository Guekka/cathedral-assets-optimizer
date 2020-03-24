/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "BSASplit.hpp"
#include "Settings/Profiles.hpp"
#include "Utils/Filesystem.hpp"

namespace CAO {
std::vector<BSA> BSASplit::splitBSA(const QDir &dir, const GeneralSettings &generalSets)
{
    std::vector<BSA> bsas{BSA::getBSA(StandardBsa, generalSets),
                          BSA::getBSA(UncompressableBsa, generalSets),
                          BSA::getBSA(TexturesBsa, generalSets)};

    auto *standardBsa = &bsas[0];
    auto *uncompressableBsa = &bsas[1];
    auto *texturesBsa = &bsas[2];

    QDirIterator it(dir, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        it.next();

        if (!isAllowedFile(dir, it.fileInfo()))
            continue;

        const auto &patternSettings = Profiles().getCurrent().getSettings(it.filePath());

        BSA **pBsa = patternSettings.bBSAIsTexture() ? &texturesBsa : &standardBsa;
        pBsa = patternSettings.bBSAIsUncompressible() ? &uncompressableBsa : pBsa;

        //adding files and sizes to list
        (*pBsa)->files << it.filePath();
        (*pBsa)->filesSize += it.fileInfo().size();

        if ((*pBsa)->filesSize >= (*pBsa)->maxSize)
        {
            bsas.emplace_back(BSA::getBSA((*pBsa)->type, generalSets));
            *pBsa = &bsas.back();
        }
    }

    //Merging BSAs that can be merged
    if (generalSets.bBSACompact())
        BSA::mergeBSAs(bsas);

    return bsas;
}

bool BSASplit::isAllowedFile(const QDir &bsaDir, const QFileInfo &fileinfo)
{
    if (fileinfo.isDir())
        return false;

    //Removing files at the root directory, those cannot be packed
    if (bsaDir.absoluteFilePath(fileinfo.fileName()) == fileinfo.absoluteFilePath())
        return false;

    const auto &settings = Profiles().getCurrent().getSettings(fileinfo.filePath());

    return settings.bBSAAddToBSA()
           && (settings.bBSAIsTexture() || settings.bBSAIsStandard()
               || settings.bBSAIsUncompressible());
}

//TODO Move files to not pack to patterns

} // namespace CAO
