/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Utils/Filesystem.hpp"
#include "Commands/Plugins/PluginsOperations.hpp"

namespace CAO {
void Filesystem::deleteEmptyDirectories(const QString &folderPath)
{
    QDirIterator dirIt(folderPath, QDirIterator::Subdirectories);
    QMap<int, QStringList> dirs;

    while (dirIt.hasNext())
    {
        QString path = QDir::cleanPath(dirIt.next());
        int size     = path.size();

        const bool alreadyExist = dirs[size].contains(path);
        const bool isSeparator  = path.contains("separator", Qt::CaseInsensitive);

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

bool Filesystem::compareFolders(const QString &folder1, const QString &folder2)
{
    QDirIterator it1(folder1, QDirIterator::Subdirectories);
    QDirIterator it2(folder2, QDirIterator::Subdirectories);

    const QDir dir1(folder1);
    const QDir dir2(folder2);

    while (it1.hasNext() && it1.hasNext())
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

void Filesystem::copyDir(const QDir &source, QDir destination, const bool overwriteExisting)
{
    QDirIterator it(source, QDirIterator::Subdirectories);

    PLOG_DEBUG << QString("Copying directory: '%1' into '%2'\nOverwrite existing:%3")
                      .arg(source.path(), destination.path(), QString::number(overwriteExisting));

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

QByteArray Filesystem::fileChecksum(QFile &&file, QCryptographicHash::Algorithm hashAlgorithm)
{
    if (file.open(QFile::ReadOnly))
    {
        QCryptographicHash hash(hashAlgorithm);
        if (hash.addData(&file))

            return hash.result();
    }
    return QByteArray();
}

bool Filesystem::compareFiles(const QString &filepath1, const QString &filepath2)
{
    QFile file1(filepath1);
    QFile file2(filepath2);

    if (file1.size() != file2.size())
        return false;

    auto hash1 = fileChecksum(std::move(file1), QCryptographicHash::Sha1);
    auto hash2 = fileChecksum(std::move(file2), QCryptographicHash::Sha1);
    return hash1 == hash2;
}

QString Filesystem::backupFile(const QString &filePath)
{
    QFile backupFile(filePath + ".bak");
    QFile file(filePath);

    while (backupFile.exists())
    {
        if (compareFiles(backupFile.fileName(), filePath))
            return backupFile.fileName(); //Backup exists and is the same as the current file
        else
            backupFile.setFileName(backupFile.fileName() + ".bak"); //Backup already exists but is different
    }

    QFile::copy(filePath, backupFile.fileName());
    PLOG_VERBOSE << QString("Backup-ed file: '%1' to '%2'").arg(filePath, backupFile.fileName());
    return backupFile.fileName();
}

QStringList Filesystem::readFile(QFile &file, std::function<void(QString &line)> function)
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

QStringList Filesystem::listPlugins(QDirIterator &it)
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

std::fstream Filesystem::openBinaryFile(const QString &filepath)
{
    std::fstream file;
    namespace fs = std::filesystem;
    file.open(fs::u8path(filepath.toStdString()), std::ios::binary | std::ios::in);
    return file;
}
} // namespace CAO
