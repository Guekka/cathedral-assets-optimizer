/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"
namespace CAO {
class FileTypes;
/*!
 * \brief Manages filesystem operations : moving files, deleting empty dirs...
 */
namespace Filesystem {
/*!
  * \brief Delete empty directories in the given directory
  * \param folderPath The path of the folder where empty dirs will be deleted
  */
void deleteEmptyDirectories(const QString &folderPath, const FileTypes &filetypes);
/*!
   * \brief Compares if two folders have the same file structure. Currently only used for testing.
   * \param folder1 The first folder
   * \param folder2 The second folder
   * \param checkFileSize Whether file sizes will be checked or not
   * \return a bool : true if the folders are identical, false otherwise
   */
bool compareFolders(const QString &folder1, const QString &folder2);
/*!
   * \brief Will copy all files from source folder into destination folder.Currently only used for testing.
   * \param source The source directory
   * \param destination The destination directory
   * \param overwriteExisting If enabled, source files will overwrite destination files
   */
void copyDir(const QDir &source, QDir destination, bool overwriteExisting);

/*!
     * \brief Renames a file in order to add .bak at the end. If an identical bak file exists, it will be deleted. If a different bak file exists, 
     * it will be renamed to .bak.bak etc
     * \param filePath The file to backup
     * \return The new path of the file
     */
QString backupFile(const QString &filePath);

QStringList readFile(
    QFile &file, const std::function<void(QString &line)> &function = [](QString &) {});

QStringList listPlugins(QDirIterator &it);

std::fstream openBinaryFile(const QString &filepath);

bool compareFiles(const QString &filepath1, const QString &filepath2);

QByteArray fileChecksum(QFile &&file, QCryptographicHash::Algorithm hashAlgorithm);

}; // namespace Filesystem
} // namespace CAO
