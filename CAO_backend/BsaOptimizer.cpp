/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "BsaOptimizer.h"

BsaOptimizer::BsaOptimizer()
{
    //Reading filesToNotPack to add them to the list.
    //Done in the constructor since the file won't change at runtime.

    QFile filesToNotPackFile("resources/FilesToNotPack.txt");
    if(filesToNotPackFile.open(QIODevice::ReadOnly))
    {
        QTextStream ts(&filesToNotPackFile);
        while (!ts.atEnd())
        {
            QString readLine = ts.readLine();
            if(readLine.left(1) != "#" && !readLine.isEmpty())
                filesToNotPack << readLine;
        }
    }
    else
        QLogger::QLog_Warning("BsaOptimizer", tr("FilesToNotPack.txt not found. Animations will be packed, preventing them from being detected by FNIS and Nemesis."));
}

void BsaOptimizer::extract(QString bsaPath, const bool &deleteBackup)
{
    bsaPath = backup(bsaPath);

    QString bsaRoot(QFileInfo(bsaPath).path());
    BSArchiveAuto archive(bsaRoot);

    try {
        archive.open(bsaPath);
        archive.extractAll(bsaRoot, false);
    } catch (std::exception& e) {
        QLogger::QLog_Error("BsaOptimizer", e.what());
        QLogger::QLog_Error("BsaOptimizer", tr("An error occured during the extraction of: ") + bsaPath + "\n" + tr("Please extract it manually. The BSA was not deleted."));
        archive.close();
        return;
    }
    archive.close();

    if(deleteBackup)
        QFile::remove(bsaPath);

    QLogger::QLog_Info("BsaOptimizer", tr("BSA successfully extracted: ") + bsaPath);
}


void BsaOptimizer::create(Bsa bsa)
{
    QDir bsaDir(QFileInfo(bsa.path).path());

    //Checking if a bsa already exists

    if(QFile(bsa.path).exists())
    {
        QLogger::QLog_Error("BsaOptimizer", tr("Cannot pack existing loose files: a BSA already exists."));
        return;
    }

    BSArchiveAuto archive(bsaDir.path());
    archive.setShareData(true);
    archive.setCompressed(true);

    //Detecting if BSA will contain sounds, since compressing BSA breaks sounds. Same for strings, Wrye Bash complains

    for (const auto& file: bsa.files)
    {
        bool isCompressed = true;
        if(isCompressed)
            if(!canBeCompressedFile(file))
            {
                archive.setCompressed(false);
                isCompressed = false;
            }

        QString filename = QFileInfo(file).fileName();

        if(bsaDir.filePath(filename) == file)
            bsa.files.removeAll(file);
    }


    QLogger::QLog_Debug("BsaOptimizer", "\nBSA folder :" + bsa.path + "\nBsaName : " + bsa.path + "\nBSAFilesSize: "
                        + QString::number(bsa.filesSize));

    try {
        archive.addFileFromDiskRoot(bsa.files);
    } catch (const std::exception& e) {
        QLogger::QLog_Error("BsaOptimizer", e.what());
        QLogger::QLog_Error("BsaOptimizer", "Cancelling packing of: " + bsa.path);
        archive.close();
        return;
    }

    //Creating the archive

    try {
        archive.create(bsa.path, bsa.format);
        archive.save();
        archive.close();
    } catch (const std::exception& e) {
        QLogger::QLog_Error("BsaOptimizer", e.what());
        QLogger::QLog_Error("BsaOptimizer", "Cancelling packing of: " + bsa.path);
        archive.close();
        return;
    }

    //Checking if the archive is below 2.15gb, since a BSA cannot be greater

    //TODO different max size before and after compression
    if(QFile(bsa.path).size() < bsa.maxSize)
    {
        QLogger::QLog_Note("BsaOptimizer", tr("BSA successfully compressed: ") + bsa.path);
        for (const auto& file : bsa.files)
            QFile::remove(file);
    }
    else
    {
        QLogger::QLog_Error("BsaOptimizer", tr("The BSA was not compressed: it is over 2.15gb: ") + bsa.path);
        QFile::remove(bsa.path);
    }
}

void BsaOptimizer::packAll(const QString &folderPath)
{
    QLogger::QLog_Trace("BsaOptimizer", "Entering " + QString(__FUNCTION__) + " function"
                        + "Packing all loose files into BSAs");

    Bsa texturesBsa, standardBsa;
    //Setting type
    texturesBsa.type = bsaType::texturesBsa;
    standardBsa.type = bsaType::standardBsa;

    //Naming BSAs
    texturesBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, texturesBsa.type) + CAO_BSA_TEXTURES_SUFFIX;
    standardBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, standardBsa.type) + CAO_BSA_SUFFIX;

    QLogger::QLog_Debug("BsaOptimizer", standardBsa.path);
    QLogger::QLog_Debug("BsaOptimizer", folderPath + "/" + PluginsOperations::findPlugin(folderPath, standardBsa.type) + CAO_BSA_SUFFIX);


    //Setting maxsize
    texturesBsa.maxSize = CAO_BSA_TEXTURES_MAX_SIZE ;
    standardBsa.maxSize = CAO_BSA_MAX_SIZE;

    //Setting format
    texturesBsa.format = CAO_BSA_TEXTURES_FORMAT;
    standardBsa.format = CAO_BSA_FORMAT;

    QDirIterator it(folderPath, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        it.next();
        QFileInfo currentFile (it.filePath());
        bool doNotPack = isIgnoredFile(it.fileName()) || currentFile.isDir();
        if(allAssets.contains(it.fileName().right(3), Qt::CaseInsensitive) && !doNotPack)
        {
            bool isTexture = texturesAssets.contains(it.fileName().right(3)) && CAO_HAS_BSA_TEXTURES; ; //If false, it means that it's a "standard" asset
            Bsa &pBsa = isTexture ? texturesBsa : standardBsa; //Using references to avoid duplicating the code

            if(pBsa.filesSize > pBsa.maxSize)
            {
                //Each time the maximum size is reached, a BSA is created

                if(isTexture)
                    pBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, texturesBsa.type) + CAO_BSA_TEXTURES_SUFFIX;
                else
                    pBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, standardBsa.type) + CAO_BSA_SUFFIX;

                create(pBsa);

                //Resetting for next loop
                pBsa.filesSize = 0;
                pBsa.files.clear();
            }
            //adding files and sizes to list
            pBsa.files << currentFile.absoluteFilePath();
            pBsa.filesSize += currentFile.size();
        }
    }

    //Since the maximum size wasn't reached for the last archive, some files are still unpacked
    if (!texturesBsa.files.isEmpty())
    {
        texturesBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, bsaType::texturesBsa) + CAO_BSA_TEXTURES_SUFFIX;
        create(texturesBsa);
    }
    if (!standardBsa.files.isEmpty())
    {
        standardBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, bsaType::standardBsa) + CAO_BSA_SUFFIX;
        create(standardBsa);
    }
}


QString BsaOptimizer::backup(const QString& bsaPath) const
{
    QFile bsaBackupFile(bsaPath + ".bak");
    QFile bsaFile(bsaPath);

    if(!bsaBackupFile.exists())
        QFile::rename(bsaPath, bsaBackupFile.fileName());
    else
    {
        if(bsaFile.size() == bsaBackupFile.size())
            QFile::remove(bsaBackupFile.fileName());
        else
            QFile::rename(bsaBackupFile.fileName(), bsaBackupFile.fileName() + ".bak");
    }

    QFile::rename(bsaPath, bsaBackupFile.fileName());

    return bsaBackupFile.fileName();
}

bool BsaOptimizer::isIgnoredFile(const QString &filepath) const
{
    for(const auto& fileToNotPack : filesToNotPack)
    {
        if(filepath.contains(fileToNotPack, Qt::CaseInsensitive))
            return true;
    }
    return  false;
}

bool BsaOptimizer::canBeCompressedFile(const QString &filename) const
{
    if(filename.endsWith(".wav", Qt::CaseInsensitive) || filename.endsWith(".xwm", Qt::CaseInsensitive)
            || filename.contains(QRegularExpression("^.+\\.[^.]*strings$")))
    {
        return false;
    }
    return true;
}
