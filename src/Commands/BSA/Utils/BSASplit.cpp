/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "BSASplit.hpp"
#include "Settings/Profiles.hpp"
#include "Utils/Algorithms.hpp"
#include "Utils/Filesystem.hpp"
#include "Utils/wildcards.hpp"

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

        const std::string &name = it.filePath().toStdString();
        auto match              = [&name](const std::string &str) {
            using namespace wildcards;
            return isMatch(name, pattern{str}, case_insensitive);
        };

        const auto &fileTypes       = currentProfile().getFileTypes();
        const bool isTexture        = any_of(fileTypes.slBSATextureFiles(), match);
        const bool isUncompressible = any_of(fileTypes.slBSAUncompressibleFiles(), match);

        BSA **pBsa = isTexture ? &texturesBsa : &standardBsa;
        pBsa       = isUncompressible ? &uncompressableBsa : pBsa;

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
    BSA::mergeBSAs(bsas, generalSets.bBSACompact());

    return bsas;
}

bool BSASplit::isAllowedFile(const QDir &bsaDir, const QFileInfo &fileinfo)
{
    if (fileinfo.isDir())
        return false;

    //Removing files at the root directory, those cannot be packed
    if (bsaDir.absoluteFilePath(fileinfo.fileName()) == fileinfo.absoluteFilePath())
        return false;

    const std::string &name = fileinfo.fileName().toStdString();
    auto match              = [&name](const std::string &str) {
        using namespace wildcards;
        return isMatch(name, pattern{str}, case_insensitive);
    };

    const auto &types = currentProfile().getFileTypes();

    return any_of(types.slBSAStandardFiles(), match) || any_of(types.slBSATextureFiles(), match)
           || any_of(types.slBSAUncompressibleFiles(), match);
}

} // namespace CAO
