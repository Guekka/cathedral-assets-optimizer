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

    QFile filesToNotPackFile("resources/FilesToNotPack.txt");
    if (filesToNotPackFile.open(QIODevice::ReadOnly))
    {
        QTextStream ts(&filesToNotPackFile);
        while (!ts.atEnd())
        {
            QString readLine = ts.readLine();
            if (readLine.left(1) != "#" && !readLine.isEmpty())
                filesToNotPack << readLine;
        }
    }
    else
        PLOG_ERROR << tr("FilesToNotPack.txt not found. Animations will be packed, preventing them from being detected "
                         "by FNIS and Nemesis.");
}

void BsaOptimizer::extract(QString bsaPath, const bool &deleteBackup) const
{
    bsaPath = backup(bsaPath);

    const QString bsaRoot(QFileInfo(bsaPath).path());
    BSArchiveAuto archive(bsaRoot);

    try
    {
        archive.open(bsaPath);
        archive.extractAll(bsaRoot, false);
    }
    catch (std::exception &e)
    {
        PLOG_ERROR << e.what();
        PLOG_ERROR << tr("An error occured during the extraction of: ") + bsaPath + "\n"
                          + tr("Please extract it manually. The BSA was not deleted.");
        archive.close();
        return;
    }
    archive.close();

    if (deleteBackup)
        QFile::remove(bsaPath);

    PLOG_INFO << tr("BSA successfully extracted: ") + bsaPath;
}

void BsaOptimizer::create(Bsa &bsa) const
{
    const QDir bsaDir(QFileInfo(bsa.path).path());

    //Checking if a bsa already exists

    if (QFile(bsa.path).exists())
    {
        PLOG_ERROR << tr("Cannot pack existing loose files: a BSA already exists.");
        return;
    }

    BSArchiveAuto archive(bsaDir.path());
    archive.setShareData(true);
    archive.setCompressed(true);

    //Detecting if BSA will contain sounds, since compressing BSA breaks sounds. Same for strings, Wrye Bash complains

    bool isCompressed = true;
    for (const auto &file : bsa.files)
    {
        if (isCompressed)
            if (!canBeCompressedFile(file))
            {
                archive.setCompressed(false);
                isCompressed = false;
            }

        //Removing files at the root directory, those cannot be packed
        QString filename = QFileInfo(file).fileName();

        if (bsaDir.filePath(filename) == file)
            bsa.files.removeAll(file);
    }

    PLOG_DEBUG << "\nBSA folder :" + bsa.path + "\nBsaName : " + bsa.path
                      + "\nBSAFilesSize: " + QString::number(bsa.filesSize);

    try
    {
        archive.addFileFromDiskRoot(bsa.files);
    }
    catch (const std::exception &e)
    {
        PLOG_ERROR << e.what() << "Cancelling packing of: " + bsa.path;
        archive.close();
        return;
    }

    //Creating the archive

    try
    {
        archive.create(bsa.path, bsa.format);
        archive.save();
        archive.close();
    }
    catch (const std::exception &e)
    {
        PLOG_ERROR << e.what();
        PLOG_ERROR << "Cancelling packing of: " + bsa.path;
        archive.close();
        return;
    }

    //Checking if the archive is below 2.15gb, since a BSA cannot be greater

    if (QFile(bsa.path).size() < LONG_MAX)
    {
        PLOG_INFO << tr("BSA successfully compressed: ") + bsa.path;
        for (const auto &file : bsa.files)
            QFile::remove(file);
    }
    else
    {
        PLOG_ERROR << tr("The BSA was not compressed: it is over 2.15gb: ") + bsa.path;
        QFile::remove(bsa.path);
    }
}

void BsaOptimizer::packAll(const QString &folderPath) const
{
    PLOG_VERBOSE << "Entering" __FUNCTION__ "function."
                                            "Packing all loose files into BSAs";

    Bsa texturesBsa, standardBsa;
    //Setting type
    texturesBsa.type = BsaType::TexturesBsa;
    standardBsa.type = BsaType::StandardBsa;

    //Naming BSAs
    texturesBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, texturesBsa.type)
                       + Games::bsaTexturesSuffix();
    standardBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, standardBsa.type)
                       + Games::bsaSuffix();

    //Setting maxsize
    texturesBsa.maxSize = Games::maxBsaTexturesSize();
    standardBsa.maxSize = Games::maxBsaUncompressedSize();

    //Setting format
    texturesBsa.format = Games::bsaTexturesFormat();
    standardBsa.format = Games::bsaFormat();

    QDirIterator it(folderPath, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        it.next();
        const bool doNotPack = isIgnoredFile(it.fileName()) || it.fileInfo().isDir();
        if (allAssets.contains(it.fileName().right(3), Qt::CaseInsensitive) && !doNotPack)
        {
            const bool isTexture = texturesAssets.contains(it.fileName().right(3)) && Games::hasBsaTextures();
            Bsa &pBsa = isTexture ? texturesBsa : standardBsa; //Using references to avoid duplicating the code

            if (pBsa.filesSize > pBsa.maxSize)
            {
                //Each time the maximum size is reached, a BSA is created

                if (isTexture)
                    pBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, texturesBsa.type)
                                + Games::bsaTexturesSuffix();
                else
                    pBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, standardBsa.type)
                                + Games::bsaSuffix();

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
                           + Games::bsaTexturesSuffix();
        create(texturesBsa);
    }
    if (!standardBsa.files.isEmpty())
    {
        standardBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, standardBsa.type)
                           + Games::bsaSuffix();
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
    return (filename.endsWith(".wav", Qt::CaseInsensitive) || filename.endsWith(".xwm", Qt::CaseInsensitive)
            || filename.contains(QRegularExpression("^.+\\.[^.]*strings$")));
}
