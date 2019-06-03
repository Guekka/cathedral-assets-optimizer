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

    try {
        archive.addFileFromDiskRoot(bsa.files) ;
    } catch (std::exception& e) {
        QLogger::QLog_Error("BsaOptimizer", e.what());
        QLogger::QLog_Error("BsaOptimizer", "Cancelling packing of: " + bsa.path);
        archive.close();
        return;
    }

    //Creating the archive

    try {
        archive.create(bsa.path, Games::GetBsaFormat());
        archive.save();
        archive.close();
    } catch (std::exception e) {
        QLogger::QLog_Error("BsaOptimizer", e.what());
        QLogger::QLog_Error("BsaOptimizer", "Cancelling packing of: " + bsa.path);
        archive.close();
        return;
    }

    QLogger::QLog_Debug("BsaOptimizer", "\nBSA folder :" + bsa.path + "\nBsaName : " + bsa.path + "\nBSAsize: "
                        + QString::number(QFile(bsa.path).size()));

    //Checking if the archive is below 2.15gb, since a BSA cannot be greater

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
    //Naming BSAs
    texturesBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, bsaType::texturesBsa) + " - Textures" + Games::getBsaExtension();
    standardBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, bsaType::standardBsa) + Games::getBsaExtension();

    //Setting maxsize
    texturesBsa.maxSize = Games::getBsaTexturesMaxSize();
    standardBsa.maxSize = Games::getBsaMaxSize();

    QDirIterator it(folderPath, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        it.next();
        QFileInfo currentFile (it.filePath());
        bool doNotPack = isIgnoredFile(it.fileName()) || currentFile.isDir();
        if(allAssets.contains(it.fileName().right(3), Qt::CaseInsensitive) && !doNotPack)
        {

            bool isTexture = texturesAssets.contains(it.fileName().right(3)); //If false, it means that it's a "standard" asset
            Bsa &pBsa = isTexture ? texturesBsa : standardBsa; //Using references to avoid duplicating the code

            if(pBsa.filesSize > pBsa.maxSize)
            {
                //Each time the maximum size is reached, a BSA is created

                pBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, isTexture ? bsaType::texturesBsa : bsaType::standardBsa) + Games::getBsaExtension();
                if(isTexture) pBsa.path = pBsa.path.chopped(4) + " - Textures" + Games::getBsaExtension();
                create(pBsa);

                //Resetting for next loop
                pBsa.filesSize = 0;
                pBsa.files.clear();
            }
            //adding files and sizes to list
            pBsa.files << it.filePath();
            pBsa.filesSize += currentFile.size();
        }
    }

    //Since the maximum size wasn't reached for the last archive, some files are still unpacked
    if (!texturesBsa.files.isEmpty())
    {
        texturesBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, bsaType::texturesBsa) + " - Textures" + Games::getBsaExtension();
        create(texturesBsa);
    }
    if (!standardBsa.files.isEmpty())
    {
        standardBsa.path = folderPath + "/" + PluginsOperations::findPlugin(folderPath, bsaType::standardBsa) + Games::getBsaExtension();
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

bool BsaOptimizer::isIgnoredFile(const QString &filepath)
{
    for(const auto& fileToNotPack : filesToNotPack)
    {
        if(filepath.contains(fileToNotPack))
            return true;
    }
    return  false;
}

bool BsaOptimizer::canBeCompressedFile(const QString &filename)
{
    if(filename.endsWith(".wav", Qt::CaseInsensitive) || filename.endsWith(".xwm", Qt::CaseInsensitive)
            || filename.contains(QRegularExpression("^.+\\.[^.]*strings$")))
    {
        return false;
    }
    return true;
}
