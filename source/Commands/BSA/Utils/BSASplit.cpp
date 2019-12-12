/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "BSASplit.hpp"
#include "FilesystemOperations.hpp"

namespace CAO {
QStringList BSASplit::filesToNotPack;

QVector<BSA> BSASplit::splitBSA(const QDir &dir, const Settings &settings)
{
    QVector<BSA> bsas;
    bsas << BSA::getBSA(StandardBsa, settings) << BSA::getBSA(UncompressableBsa, settings)
         << BSA::getBSA(TexturesBsa, settings);

    auto *standardBsa = &bsas[0];
    auto *uncompressableBsa = &bsas[1];
    auto *texturesBsa = &bsas[2];

    QDirIterator it(dir, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        it.next();

        if (isIgnoredFile(dir, it.fileInfo()) || !allAssets.contains(it.fileInfo().suffix(), Qt::CaseInsensitive))
            continue;

        const bool isTexture = texturesAssets.contains(it.fileInfo().suffix(), Qt::CaseInsensitive)
                               && settings.getValue<bool>(bBSATexturesEnabled);
        const bool isUncompressable = uncompressableAssets.contains(it.fileInfo().suffix(), Qt::CaseInsensitive);

        BSA **pBsa = isTexture ? &texturesBsa : &standardBsa;
        pBsa = isUncompressable ? &uncompressableBsa : pBsa;

        //adding files and sizes to list
        (*pBsa)->files << it.filePath();
        (*pBsa)->filesSize += it.fileInfo().size();

        if ((*pBsa)->filesSize >= (*pBsa)->maxSize)
        {
            bsas << BSA::getBSA((*pBsa)->type, settings);
            *pBsa = &bsas.last();
        }
    }

    //Merging BSAs that can be merged
    if (settings.getValue<bool>(bBsaLeastBsaPossible))
        BSA::mergeBSAs(bsas);

    return bsas;
}

bool BSASplit::isIgnoredFile(const QDir &bsaDir, const QFileInfo &fileinfo)
{
    if (fileinfo.isDir())
        return true;

    for (const auto &fileToNotPack : filesToNotPack)
    {
        if (fileinfo.absoluteFilePath().contains(fileToNotPack, Qt::CaseInsensitive))
            return true;
    }

    //Removing files at the root directory, those cannot be packed
    if (bsaDir.absoluteFilePath(fileinfo.fileName()) == fileinfo.absoluteFilePath())
        return true;

    return false;
}

void BSASplit::readFilesToNotPack()
{
    QFile &&filesToNotPackFile = Profiles::getFile("FilesToNotPack.txt");

    filesToNotPack = FilesystemOperations::readFile(filesToNotPackFile,
                                                    [](QString &line) { line = QDir::fromNativeSeparators(line); });
    if (filesToNotPack.isEmpty())
    {
        PLOG_ERROR << "FilesToNotPack.txt not found. This can cause a number of issues. For example, for Skyrim, "
                      "animations will be packed to BSA, preventing them from being detected "
                      "by FNIS and Nemesis.";
    }
}

} // namespace CAO
