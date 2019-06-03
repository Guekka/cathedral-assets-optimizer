/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#include "PluginsOperations.h"


const QStringList texturesAssets{ "png", "dds" };
const QStringList otherAssets{ "nif", "seq", "pex", "psc", "lod", "fuz", "wav", "xwm", "swf", "hkx", "tri", "btr", "bto", "btt", "lip", "txt", "lst" };
const QStringList allAssets = texturesAssets + otherAssets;

/*!
 * \brief Manages filesystem operations : moving files, deleting empty dirs...
 */
class FilesystemOperations : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(FilesystemOperations)

public:
    /*!
     * \brief Will move all files from source folder into destination folder.
     * \param source The source directory
     * \param destination The destination directory
     * \param overwriteExisting If enabled, source files will overwrite destination files
     */
    static bool moveFiles(const QString& source, const QString& destination, bool overwriteExisting);
    /*!
     * \brief Will find skyrim directory using the registry key.
     * \return A QString containing the path of the Skyrim directory, or an empty QString if the path is not found
     */
    static QString findSkyrimDirectory();
    /*!
    * \brief Delete empty directories in the given directory
    * \param folderPath The path of the folder where empty dirs will be deleted
    */
    static void deleteEmptyDirectories(const QString &folderPath);
    /*!
     * \brief Compares if two folders have the same file structure. Currently only used for testing.
     * \param folder1 The first folder
     * \param folder2 The second folder
     * \param checkFileSize Wheter file sizes will be checked or not
     * \return a bool : true if the folders are identical, false otherwise
     */
    static bool compareFolders(const QString& folder1, const QString& folder2, const bool& checkFileSize);
    /*!
     * \brief Will copy all files from source folder into destination folder.Currently only used for testing.
     * \param source The source directory
     * \param destination The destination directory
     * \param overwriteExisting If enabled, source files will overwrite destination files
     */
    static void copyDir(const QString& source, const QString& destination, bool overwriteExisting);

};
