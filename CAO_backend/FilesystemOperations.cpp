/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "FilesystemOperations.h"

FilesystemOperations::FilesystemOperations() {}

bool FilesystemOperations::moveFiles(const QString& source, const QString& destination, bool overwriteExisting)
{
    QDir sourceDir(source);
    QDir destinationDir(destination);
    QDirIterator it(source, QDirIterator::Subdirectories);

    QLogger::QLog_Trace("FilesystemOperations", "Entering " + QString(__FUNCTION__) + " function");
    QLogger::QLog_Debug("FilesystemOperations", "dest folder: " + destination + "\nsource folder: " + source);

    QStringList oldFiles;

    //Listing all files

    while (it.hasNext())
    {
        it.next();
        if(!QFileInfo(it.filePath()).isDir()) //Skipping all directories and avoiding to copy from the destination folder
            oldFiles << it.filePath();
    }

    oldFiles.removeDuplicates();

    //Actually moving files

    for (const auto& oldFile : oldFiles)
    {
        QString relativeFilename = sourceDir.relativeFilePath(oldFile);
        QString newFileRelativeFilename = destinationDir.relativeFilePath(QDir::cleanPath(destination + QDir::separator() + relativeFilename));

        if(newFileRelativeFilename.size() >= 255 || oldFile.size() >=255)
        {
            QLogger::QLog_Error("FilesystemOperations", tr("An error occurred while moving files. Try reducing path size (260 characters is the maximum)"));
            QLogger::QLog_Error("Errors", tr("An error occurred while moving files. Try reducing path size (260 characters is the maximum)"));
            return false;
        }

        //removing the duplicate files from new folder (if overwriteExisting) or from old folder (if !overwriteExisting)

        destinationDir.mkpath(QFileInfo(newFileRelativeFilename).path());

        if(overwriteExisting)
            destinationDir.remove(newFileRelativeFilename);

        destinationDir.rename(oldFile, newFileRelativeFilename);

        if(!overwriteExisting)
            destinationDir.remove(oldFile);
    }
    QLogger::QLog_Trace("FilesystemOperations", "Exiting moveFiles function");
    return true;
}

void FilesystemOperations::deleteEmptyDirectories(const QString& folderPath)
{
    QDirIterator dirIt(folderPath, QDirIterator::Subdirectories);
    QMap<int, QStringList> dirs;

    while (dirIt.hasNext())
    {
        QString path = QDir::cleanPath(dirIt.next());
        int size = path.size();

        bool alreadyExist = dirs[size].contains(path);
        bool isSeparator = path.contains("separator", Qt::CaseInsensitive);

        if(!alreadyExist && !isSeparator)
            dirs[size].append(path);
    }

    QDir dir(folderPath);
    QMapIterator<int, QStringList> i(dirs);

    i.toBack();
    while (i.hasPrevious())
    {
        i.previous();
        for (int j = 0; j < i.value().size(); ++j)
        {
            dir.rmpath(i.value().at(j));
        }
    }
}

bool FilesystemOperations::compareFolders(const QString& folder1, const QString& folder2, const bool& checkFileSize)
{
    QDirIterator it1(folder1, QDirIterator::Subdirectories);
    QDirIterator it2(folder2, QDirIterator::Subdirectories);

    QStringList files1;
    QStringList files2;

    QDir dir1(folder1);
    QDir dir2(folder2);

    QVector<qint64> filesSize1;
    QVector<qint64> filesSize2;

    while (it1.hasNext())
    {
        QString currentFile = dir1.relativeFilePath(it1.next());
        files1 << currentFile;

        if(checkFileSize)
            filesSize1 << QFile(currentFile).size();
    }

    while (it2.hasNext())
    {
        QString currentFile = dir2.relativeFilePath(it2.next());
        files2 << currentFile;

        if(checkFileSize)
            filesSize2 << QFile(currentFile).size();
    }

    if(files1.size() != files2.size())
        return false;

    if(files1 != files2)
        return false;

    if(checkFileSize && filesSize1 != filesSize2)
        return false;

    return true;
}

void FilesystemOperations::copyDir(const QString& source, const QString& destination, bool overwriteExisting)
{
    QDir sourceDir(source);
    QDir destinationDir(destination);
    QDirIterator it(source, QDirIterator::Subdirectories);

    QLogger::QLog_Trace("FilesystemOperations", "Entering " + QString(__FUNCTION__) + " function");
    QLogger::QLog_Debug("FilesystemOperations", "dest folder: " + destination + "\nsource folder: " + source);

    QStringList oldFiles;

    QString currentDir = QDir::currentPath();
    QDir::setCurrent(destination);

    while (it.hasNext())
    {
        it.next();
        if(!QFileInfo(it.filePath()).isDir()) //Skipping all directories
            oldFiles << it.filePath();
    }

    oldFiles.removeDuplicates();

    for (const auto& oldFile : oldFiles)
    {
        QString relativeFilename = sourceDir.relativeFilePath(oldFile);
        QString newFile = QDir::cleanPath(destination + QDir::separator() + relativeFilename);

        if(newFile.size() >= 255)
        {
            QLogger::QLog_Error("FilesystemOperations", tr("An error occurred while moving files. Try reducing path size (260 characters is the maximum)"));
            QLogger::QLog_Error("Errors", tr("An error occurred while moving files. Try reducing path size (260 characters is the maximum)"));
            return;
        }

        destinationDir.mkpath(QFileInfo(newFile).path());

        if(overwriteExisting)
            destinationDir.remove(newFile);

        QFile::copy(oldFile, newFile);
    }
    QLogger::QLog_Trace("FilesystemOperations", "Exiting moveFiles function");

    QDir::setCurrent(currentDir);
}

QString FilesystemOperations::findSkyrimDirectory() //Find Skyrim directory using the registry key
{
    QSettings SkyrimReg(R"(HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Bethesda Softworks\Skyrim Special Edition)", QSettings::NativeFormat);
    QString SkyrimDir = QDir::cleanPath(SkyrimReg.value("Installed Path").toString());
    return SkyrimDir;
}
