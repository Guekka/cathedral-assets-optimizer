/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "BsaOptimizer.h"
#include "PluginsOperations.h"

BsaOptimizer::BsaOptimizer()
{
    //Reading filesToNotPack to add them to the list.
    //Done in the constructor since the file won't change at runtime.

    QFile &&filesToNotPackFile = Profiles::getFile("FilesToNotPack.txt");

    filesToNotPack = FilesystemOperations::readFile(filesToNotPackFile);
    if (filesToNotPack.isEmpty())
    {
        PLOG_ERROR << "FilesToNotPack.txt not found. This can cause a number of issues. For example, for Skyrim, "
                      "animations will be packed to BSA, preventing them from being detected "
                      "by FNIS and Nemesis.";
    }
}

void BsaOptimizer::extract(QString bsaPath, const bool &deleteBackup) const
{
    bsaPath = backup(bsaPath);

    auto rootPath = new QString(QFileInfo(bsaPath).path());

    try
    {
        BSArchiveAuto archive(*rootPath);
        archive.setDDSCallback(&BsaOptimizer::DDSCallback, rootPath);
        archive.open(bsaPath);
        archive.extractAll(*rootPath, false);
    }
    catch (const std::exception &e)
    {
        PLOG_ERROR << e.what();
        PLOG_ERROR << "An error occured during the extraction of: " + bsaPath + 'n'
                          + "Please extract it manually. The BSA was not deleted.";
        return;
    }

    if (deleteBackup)
        QFile::remove(bsaPath);

    PLOG_INFO << "BSA successfully extracted: " + bsaPath;
}

int BsaOptimizer::create(Bsa &bsa) const
{
    auto rootPath = new QString(QFileInfo(bsa.path).path());
    const QDir bsaDir(*rootPath);

    //Checking if a bsa already exists
    if (QFile(bsa.path).exists())
    {
        PLOG_ERROR << "Cannot pack existing loose files: a BSA already exists.";
        return 1;
    }

    BSArchiveAuto archive(*rootPath);
    archive.setShareData(true);
    archive.setCompressed(true);
    archive.setDDSCallback(&BsaOptimizer::DDSCallback, rootPath);

    //Detecting if BSA will contain sounds, since compressing BSA breaks sounds. Same for strings, Wrye Bash complains
    for (const auto &file : bsa.files)
    {
        if (!canBeCompressedFile(file))
        {
            archive.setCompressed(false);
            break;
        }
    }

    if (bsa.files.isEmpty())
        return 3;

    try
    {
        archive.addFileFromDiskRoot(bsa.files);
    }
    catch (const std::exception &e)
    {
        PLOG_ERROR << e.what() << "Cancelling packing of: " + bsa.path;
        return 2;
    }

    PLOG_DEBUG << bsa;
    //Creating the archive

    try
    {
        archive.create(bsa.path, bsa.format);
        archive.save();
    }
    catch (const std::exception &e)
    {
        PLOG_ERROR << e.what();
        PLOG_ERROR << "Cancelling packing of: " + bsa.path;
        return 2;
    }

    PLOG_INFO << "BSA successfully compressed: " + bsa.path;
    for (const auto &file : bsa.files)
        QFile::remove(file);

    return 0;
}

void BsaOptimizer::packAll(const QString &folderPath) const
{
    PLOG_VERBOSE << "Packing all loose files into BSAs";

    QVector<Bsa> bsas;
    bsas.reserve(10000);
    bsas << Bsa::getBsa(StandardBsa) << Bsa::getBsa(UncompressableBsa) << Bsa::getBsa(TexturesBsa);

    auto *standardBsa = &bsas[0];
    auto *uncompressableBsa = &bsas[1];
    auto *texturesBsa = &bsas[2];

    QDirIterator it(folderPath, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        it.next();

        if (isIgnoredFile(folderPath, it.filePath()) || it.fileInfo().isDir()
            || !allAssets.contains(it.fileInfo().suffix(), Qt::CaseInsensitive))
            continue;

        const bool isTexture = texturesAssets.contains(it.fileInfo().suffix(), Qt::CaseInsensitive)
                               && Profiles::hasBsaTextures();
        const bool isUncompressable = uncompressableAssets.contains(it.fileInfo().suffix(), Qt::CaseInsensitive);

        Bsa **pBsa = isTexture ? &texturesBsa : &standardBsa;
        pBsa = isUncompressable ? &uncompressableBsa : pBsa;

        //adding files and sizes to list
        (*pBsa)->files << it.filePath();
        (*pBsa)->filesSize += it.fileInfo().size();

        if ((*pBsa)->filesSize >= (*pBsa)->maxSize)
        {
            bsas << Bsa::getBsa((*pBsa)->type);
            *pBsa = &bsas.last();
        }
    }

    //Merging BSAs that can be merged
    Bsa::mergeBsas(bsas);

    for (int i = 0; i < bsas.size(); ++i)
    {
        Bsa::nameBsa({&bsas[i]}, folderPath);
        create(bsas[i]);
    }
}

QString BsaOptimizer::backup(const QString &bsaPath) const
{
    QFile bsaBackupFile(bsaPath + ".bak");
    const QFile bsaFile(bsaPath);

    if (!bsaBackupFile.exists())

        while (bsaBackupFile.exists())
        {
            if (bsaFile.size() == bsaBackupFile.size())
                QFile::remove(bsaBackupFile.fileName());
            else
                bsaBackupFile.setFileName(bsaBackupFile.fileName() + ".bak");
        }

    QFile::rename(bsaPath, bsaBackupFile.fileName());

    PLOG_VERBOSE << "Backuping BSA : " << bsaPath << " to " << bsaBackupFile.fileName();

    return bsaBackupFile.fileName();
}

bool BsaOptimizer::isIgnoredFile(const QString &bsaFolder, const QString &filepath) const
{
    for (const auto &fileToNotPack : filesToNotPack)
    {
        if (filepath.contains(fileToNotPack, Qt::CaseInsensitive))
            return true;
    }

    //Removing files at the root directory, those cannot be packed
    const QString &filename = QFileInfo(filepath).fileName();

    QDir bsaDir(bsaFolder);
    if (bsaDir.filePath(filename) == filepath)
        return true;

    return false;
}

bool BsaOptimizer::canBeCompressedFile(const QString &filename)
{
    const bool cantBeCompressed = (filename.endsWith(".wav", Qt::CaseInsensitive)
                                   || filename.endsWith(".xwm", Qt::CaseInsensitive)
                                   || filename.contains(QRegularExpression("^.+\\.[^.]*strings$")));
    return !cantBeCompressed;
}

void BsaOptimizer::DDSCallback([[maybe_unused]] bsa_archive_t archive,
                               const wchar_t *file_path,
                               bsa_dds_info_t *dds_info,
                               void *context)
{
    const QString &path = *static_cast<QString *>(context) + '/' + QString::fromWCharArray(file_path);

    auto image = std::make_unique<DirectX::ScratchImage>();
    DirectX::TexMetadata info;

    const auto hr = LoadFromDDSFile(PREPARE_PATH_LIBBSARCH(path), DirectX::DDS_FLAGS_BAD_DXTN_TAILS, &info, *image);
    if (FAILED(hr))
    {
        PLOG_ERROR << "Failed to open DDS when packing archive: " << path;
        return;
    }

    dds_info->width = info.width;
    dds_info->height = info.height;
    dds_info->mipmaps = info.mipLevels;
}
