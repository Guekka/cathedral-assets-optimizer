/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <QDir>
#include <QDirIterator>

#include "BSASplit.hpp"
#include "Commands/BSA/Utils/BSA.hpp"
#include "File/BSA/BSAFolder.hpp"
#include "Settings/GeneralSettings.hpp"
#include "Settings/Profiles.hpp"
#include "Utils/Algorithms.hpp"
#include "Utils/Filesystem.hpp"
#include "Utils/wildcards.hpp"

namespace CAO::BSASplit {

bool isAllowedFile(const QDir &bsaDir, const QFileInfo &settings);

std::vector<BSA> splitBSA(const QDir &dir, const GeneralSettings &generalSets)
{
    std::vector<BSA> bsas{BSA::getBSA(StandardBsa, generalSets),
                          BSA::getBSA(UncompressableBsa, generalSets),
                          BSA::getBSA(TexturesBsa, generalSets)};

    auto *standardBsa       = &bsas[0];
    auto *uncompressableBsa = &bsas[1];
    auto *texturesBsa       = &bsas[2];

    QDirIterator it(dir, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        it.next();

        if (!isAllowedFile(dir, it.fileInfo()))
            continue;

        auto &ft                    = currentProfile().getFileTypes();
        const bool isTexture        = ft.match(ft.slBSATextureFiles(), it.filePath());
        const bool isUncompressible = ft.match(ft.slBSAUncompressibleFiles(), it.filePath());

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

    //Removing empty BSAs
    bsas = bsas | rx::filter([](const BSA &bsa) { return !bsa.files.empty(); }) | rx::to_vector();

    //Merging BSAs that can be merged
    BSA::MergeBSATypes mergeTypes;
    if (!generalSets.bBSADontMergeIncomp() && !generalSets.bBSADontMergeTextures())
        mergeTypes = BSA::MergeBSATypes::Both;
    else if (!generalSets.bBSADontMergeIncomp())
        mergeTypes = BSA::MergeBSATypes::Incompressible;
    else if (!generalSets.bBSADontMergeTextures())
        mergeTypes = BSA::MergeBSATypes::Textures;
    else
        mergeTypes = BSA::MergeBSATypes::None;

    BSA::mergeBSAs(bsas, mergeTypes);

    return bsas;
}

bool isAllowedFile(const QDir &bsaDir, const QFileInfo &fileinfo)
{
    if (fileinfo.isDir())
        return false;

    //Removing files at the root directory, those cannot be packed
    if (bsaDir.absoluteFilePath(fileinfo.fileName()) == fileinfo.absoluteFilePath())
        return false;

    const auto &ft      = currentProfile().getFileTypes();
    const QString &path = fileinfo.filePath();

    if (ft.match(ft.slBSABlacklist(), path))
        return false;

    return ft.match(ft.slBSATextureFiles(), path) || ft.match(ft.slBSAStandardFiles(), path)
           || ft.match(ft.slBSAUncompressibleFiles(), path);
}

} // namespace CAO::BSASplit
