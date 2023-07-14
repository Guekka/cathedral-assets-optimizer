/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Utils/Filesystem.hpp"

#include <Logger.hpp>

#include <QCryptographicHash>
#include <QDir>
#include <QDirIterator>
#include <QMap>
#include <QMapIterator>
#include <QRegularExpression>
#include <QTextCodec>
#include <QTextStream>
#include <QVariant>

namespace cao::Filesystem {
QByteArray fileChecksum(QFile &&file, QCryptographicHash::Algorithm hashAlgorithm);

void deleteEmptyDirectories(const QString &folderPath)
{
    QDirIterator dirIt(folderPath, QDirIterator::Subdirectories);
    QMap<int, QStringList> dirs;

    while (dirIt.hasNext())
    {
        QString path = QDir::cleanPath(dirIt.next());
        int size     = path.size();

        const bool alreadyExist = dirs[size].contains(path);

        if (!alreadyExist)
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

bool compareFolders(const QString &folder1, const QString &folder2)
{
    QDirIterator it1(folder1, QDirIterator::Subdirectories);
    QDirIterator it2(folder2, QDirIterator::Subdirectories);

    const QDir dir1(folder1);
    const QDir dir2(folder2);

    while (it1.hasNext() && it2.hasNext())
    {
        it1.next();
        it2.next();

        if (it1.fileInfo().size() != it2.fileInfo().size())
            return false;

        QString file1 = dir1.relativeFilePath(it1.filePath());
        QString file2 = dir2.relativeFilePath(it2.filePath());

        if (file1 != file2)
            return false;
    }

    if (it1.hasNext() || it2.hasNext())
        return false; //One dir has more files than the other

    return true;
}

void copyDir(const QDir &source, QDir destination, const bool overwriteExisting)
{
    QDirIterator it(source, QDirIterator::Subdirectories);

    //PLOG_DEBUG << QString("Copying directory: '%1' into '%2'\nOverwrite existing:%3")
    // FIXME                 .arg(source.path(), destination.path(), QVariant(overwriteExisting).toString());

    QStringList oldFiles;

    const QString &currentDir = QDir::currentPath();
    QDir::setCurrent(destination.path()); //Might help with 260chars limit

    while (it.hasNext())
    {
        it.next();
        if (!it.fileInfo().isDir()) //Skipping all directories
            oldFiles << it.filePath();
    }

    for (const auto &oldFile : oldFiles)
    {
        const QString &relativePath = source.relativeFilePath(oldFile);
        const QString &newFile      = destination.relativeFilePath(relativePath);

        if (newFile.size() >= 255)
        {
            PLOG_ERROR << "An error occurred while moving files. Try reducing path size (260 characters is "
                          "the maximum)";
            return;
        }

        destination.mkpath(QFileInfo(newFile).path());

        if (overwriteExisting)
        {
            destination.remove(newFile);
            QFile::copy(oldFile, newFile);
        }
    }
    QDir::setCurrent(currentDir);
}

QByteArray fileChecksum(QFile &&file, QCryptographicHash::Algorithm hashAlgorithm)
{
    if (file.open(QFile::ReadOnly))
    {
        QCryptographicHash hash(hashAlgorithm);
        if (hash.addData(&file))

            return hash.result();
    }
    return QByteArray();
}

bool compareFiles(const QString &filepath1, const QString &filepath2)
{
    QFile file1(filepath1);
    QFile file2(filepath2);

    if (file1.size() != file2.size())
        return false;

    const auto hash1 = fileChecksum(std::move(file1), QCryptographicHash::Sha1);
    const auto hash2 = fileChecksum(std::move(file2), QCryptographicHash::Sha1);
    return hash1 == hash2;
}

QString backupFile(const QString &filePath)
{
    QFile backupFile(filePath + ".bak");

    while (backupFile.exists())
    {
        if (compareFiles(backupFile.fileName(), filePath))
            return backupFile.fileName(); //Backup exists and is the same as the current file
        else
            backupFile.setFileName(backupFile.fileName() + ".bak"); //Backup already exists but is different
    }

    QFile::copy(filePath, backupFile.fileName());
    // FIXME PLOG_VERBOSE << QString("Backup-ed file: '%1' to '%2'").arg(filePath, backupFile.fileName());
    return backupFile.fileName();
}

QStringList readFile(QFile &file, const std::function<void(QString &line)> &function)
{
    file.open(QFile::ReadOnly);
    if (!file.isOpen())
        return {};

    QTextStream ts(&file);

    const unsigned char UTF16_BOM[2] = {255, 254};

    if (file.read(2) == QByteArray(reinterpret_cast<const char *>(UTF16_BOM)))
        ts.setCodec(QTextCodec::codecForName("UTF-16LE"));

    file.seek(0);

    QStringList list;
    while (!ts.atEnd())
    {
        QString &&line = ts.readLine().simplified();
        if (line.startsWith("#") || line.isEmpty())
            continue;

        function(line);
        list << line;
    }
    return list;
}

QStringList listPlugins(QDirIterator &it)
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

std::fstream openBinaryFile(const QString &filepath)
{
    std::fstream file;
    namespace fs = std::filesystem;
    file.open(fs::u8path(filepath.toStdString()), std::ios::binary | std::ios::in);
    return file;
}
} // namespace cao::Filesystem
