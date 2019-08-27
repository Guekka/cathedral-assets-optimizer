/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "BsaOptimizer.h"
#include "PluginsOperations.h"

//Global variable until libbsarch is fixed
static QString sCurrentBsaRootDir;

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

    const QString &bsaRoot(QFileInfo(bsaPath).path());

    try
    {
        BSArchiveAuto archive(bsaRoot);
        archive.setDDSCallback(&DDSCallback);
        archive.open(bsaPath);
        archive.extractAll(bsaRoot, false);
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
    sCurrentBsaRootDir = QFileInfo(bsa.path).path();
    const QDir bsaDir(sCurrentBsaRootDir);

    //Checking if a bsa already exists
    if (QFile(bsa.path).exists())
    {
        PLOG_ERROR << "Cannot pack existing loose files: a BSA already exists.";
        return 1;
    }

    BSArchiveAuto archive(bsaDir.path());
    archive.setShareData(true);
    archive.setCompressed(true);
    archive.setDDSCallback(&DDSCallback);

    //Detecting if BSA will contain sounds, since compressing BSA breaks sounds. Same for strings, Wrye Bash complains
    for (const auto &file : bsa.files)
    {
        if (!canBeCompressedFile(file))
        {
            archive.setCompressed(false);
            break;
        }
    }
    for (const auto &file : bsa.files)
    {
        //Removing files at the root directory, those cannot be packed
        QString filename = QFileInfo(file).fileName();

        if (bsaDir.filePath(filename) == file)
            bsa.files.removeAll(file);
    }

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

    Bsa texturesBsa, standardBsa;
    //Setting type
    texturesBsa.type = BsaType::TexturesBsa;
    standardBsa.type = BsaType::StandardBsa;

    //Naming BSAs
    texturesBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, texturesBsa.type)
                       + Profiles::bsaTexturesSuffix();
    standardBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, standardBsa.type)
                       + Profiles::bsaSuffix();

    //Setting maxsize
    texturesBsa.maxSize = Profiles::maxBsaTexturesSize();
    standardBsa.maxSize = Profiles::maxBsaUncompressedSize();

    //Setting format
    texturesBsa.format = Profiles::bsaTexturesFormat();
    standardBsa.format = Profiles::bsaFormat();

    QDirIterator it(folderPath, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        it.next();
        const bool doNotPack = isIgnoredFile(it.fileName()) || it.fileInfo().isDir();
        if (allAssets.contains(it.fileName().right(3), Qt::CaseInsensitive) && !doNotPack)
        {
            const bool isTexture = texturesAssets.contains(it.fileName().right(3)) && Profiles::hasBsaTextures();
            Bsa &pBsa = isTexture ? texturesBsa : standardBsa; //Using references to avoid duplicating the code

            if (pBsa.filesSize > pBsa.maxSize)
            {
                //Each time the maximum size is reached, a BSA is created

                if (isTexture)
                    pBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, texturesBsa.type)
                                + Profiles::bsaTexturesSuffix();
                else
                    pBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, standardBsa.type)
                                + Profiles::bsaSuffix();

                create(pBsa);

                //Resetting for next loop
                pBsa.filesSize = 0;
                pBsa.files.clear();
            }
            //adding files and sizes to list
            pBsa.files << it.filePath();
            pBsa.filesSize += it.fileInfo().size();
        }
    }

    //Since the maximum size wasn't reached for the last archive, some files are still unpacked
    if (!texturesBsa.files.isEmpty())
    {
        texturesBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, texturesBsa.type)
                           + Profiles::bsaTexturesSuffix();
        create(texturesBsa);
    }
    if (!standardBsa.files.isEmpty())
    {
        standardBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, standardBsa.type)
                           + Profiles::bsaSuffix();
        create(standardBsa);
    }
}

QString BsaOptimizer::backup(const QString &bsaPath) const
{
    const QFile bsaBackupFile(bsaPath + ".bak");
    const QFile bsaFile(bsaPath);

    if (!bsaBackupFile.exists())
        QFile::rename(bsaPath, bsaBackupFile.fileName());
    else
    {
        if (bsaFile.size() == bsaBackupFile.size())
            QFile::remove(bsaBackupFile.fileName());
        else
            QFile::rename(bsaBackupFile.fileName(), bsaBackupFile.fileName() + ".bak");
    }

    QFile::rename(bsaPath, bsaBackupFile.fileName());

    PLOG_VERBOSE << "Backuping BSA : " << bsaPath << " to " << bsaBackupFile.fileName();

    return bsaBackupFile.fileName();
}

bool BsaOptimizer::isIgnoredFile(const QString &filepath) const
{
    for (const auto &fileToNotPack : filesToNotPack)
    {
        if (filepath.contains(fileToNotPack, Qt::CaseInsensitive))
            return true;
    }
    return false;
}

bool BsaOptimizer::canBeCompressedFile(const QString &filename)
{
    const bool cantBeCompressed = (filename.endsWith(".wav", Qt::CaseInsensitive)
                                   || filename.endsWith(".xwm", Qt::CaseInsensitive)
                                   || filename.contains(QRegularExpression("^.+\\.[^.]*strings$")));
    return !cantBeCompressed;
}

void DDSCallback(bsa_archive_t archive, const wchar_t *file_path, bsa_dds_info_t *dds_info)
{
    const QString path = sCurrentBsaRootDir + QDir::separator() + QString::fromWCharArray(file_path);

    auto image = std::make_unique<DirectX::ScratchImage>();
    DirectX::TexMetadata info;

    const auto hr = LoadFromDDSFile(PREPARE_PATH_LIBBSARCH(path), DirectX::DDS_FLAGS_NONE, &info, *image);
    if (FAILED(hr))
        throw std::runtime_error("Failed to open DDS");

    dds_info->width = info.width;
    dds_info->height = info.height;
    dds_info->mipmaps = info.mipLevels;
}
