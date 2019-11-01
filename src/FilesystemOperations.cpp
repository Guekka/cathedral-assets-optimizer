/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "FilesystemOperations.hpp"
#include "PluginsOperations.hpp"
namespace CAO {
void FilesystemOperations::deleteEmptyDirectories(const QString &folderPath)
{
    QDirIterator dirIt(folderPath, QDirIterator::Subdirectories);
    QMap<int, QStringList> dirs;

    while (dirIt.hasNext())
    {
        QString path = QDir::cleanPath(dirIt.next());
        int size = path.size();

        const bool alreadyExist = dirs[size].contains(path);
        const bool isSeparator = path.contains("separator", Qt::CaseInsensitive);

        if (!alreadyExist && !isSeparator)
            dirs[size].append(path);
    }

    const QDir dir(folderPath);
    QMapIterator<int, QStringList> i(dirs);

    i.toBack();
    while (i.hasPrevious())
    {
        i.previous();
        for (int j = 0; j < i.value().size(); ++j)
            dir.rmpath(i.value().at(j));
    }
}

bool FilesystemOperations::compareFolders(const QString &folder1, const QString &folder2, const bool &checkFileSize)
{
    QDirIterator it1(folder1, QDirIterator::Subdirectories);
    QDirIterator it2(folder2, QDirIterator::Subdirectories);

    QStringList files1;
    QStringList files2;

    const QDir dir1(folder1);
    const QDir dir2(folder2);

    QVector<qint64> filesSize1;
    QVector<qint64> filesSize2;

    while (it1.hasNext())
    {
        QString currentFile = dir1.relativeFilePath(it1.next());
        files1 << currentFile;

        if (checkFileSize)
            filesSize1 << QFile(currentFile).size();
    }

    while (it2.hasNext())
    {
        QString currentFile = dir2.relativeFilePath(it2.next());
        files2 << currentFile;

        if (checkFileSize)
            filesSize2 << QFile(currentFile).size();
    }

    if (files1.size() != files2.size())
        return false;

    if (files1 != files2)
        return false;

    if (checkFileSize && filesSize1 != filesSize2)
        return false;

    return true;
}

void FilesystemOperations::copyDir(const QString &source, const QString &destination, const bool overwriteExisting)
{
    const QDir sourceDir(source);
    QDir destinationDir(destination);
    QDirIterator it(source, QDirIterator::Subdirectories);

    PLOG_VERBOSE << "Entering " + QString(__FUNCTION__) + " function";
    PLOG_DEBUG << "dest folder: " + destination + "\nsource folder: " + source;

    QStringList oldFiles;

    const QString currentDir = QDir::currentPath();
    QDir::setCurrent(destination);

    while (it.hasNext())
    {
        it.next();
        if (!it.fileInfo().isDir()) //Skipping all directories
            oldFiles << it.filePath();
    }

    oldFiles.removeDuplicates();

    for (const auto &oldFile : oldFiles)
    {
        QString relativeFilename = sourceDir.relativeFilePath(oldFile);
        QString newFile = QDir::cleanPath(destination + QDir::separator() + relativeFilename);

        if (newFile.size() >= 255)
        {
            PLOG_ERROR
                << "An error occurred while moving files. Try reducing path size (260 characters is the maximum)";
            return;
        }

        destinationDir.mkpath(QFileInfo(newFile).path());

        if (overwriteExisting)
            destinationDir.remove(newFile);

        QFile::copy(oldFile, newFile);
    }
    PLOG_VERBOSE << "Exiting moveFiles function";

    QDir::setCurrent(currentDir);
}

QStringList FilesystemOperations::readFile(QFile &file, std::function<void(QString &line)> function)
{
    QStringList list;

    file.open(QFile::ReadOnly);
    if (!file.isOpen())
        return list;

    while (!file.atEnd())
    {
        QString &&line = file.readLine().simplified();
        if (line.startsWith("#") || line.isEmpty())
            continue;

        function(line);
        list << line;
    }
    return list;
}

QStringList FilesystemOperations::readFile(QFile &file)
{
    QStringList list;

    file.open(QFile::ReadOnly);
    if (!file.isOpen())
        return list;

    while (!file.atEnd())
    {
        QString &&line = file.readLine().simplified();
        if (line.startsWith("#") || line.isEmpty())
            continue;

        list << line;
    }
    return list;
}

QStringList FilesystemOperations::listPlugins(QDirIterator &it)
{
    QStringList plugins;
    const QRegularExpression pluginsExt("\\.es[plm]$");
    while (it.hasNext())
    {
        it.next();
        if (it.fileName().contains(pluginsExt) && !it.fileInfo().isDir())
            plugins << it.filePath();
    }

    return plugins;
}
} // namespace CAO
