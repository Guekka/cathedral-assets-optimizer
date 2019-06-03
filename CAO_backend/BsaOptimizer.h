/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#include "FilesystemOperations.h"

struct Bsa
{
    QString path;
    qint64 filesSize = 0;
    QStringList files;
    double maxSize = LONG_MAX;
    bsaType type = bsaType::standardBsa;
};

/*!
 * \brief Manages BSA : extract and create them
 */
class BsaOptimizer : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(BsaOptimizer)

public:
    /*!
     * \brief Default constructor
     */
    BsaOptimizer();
    /*!
     * \brief Will extract a BSA
     * \param bsaPath The path of the BSA to extract
     * \param deleteBackup Deletes the backup the existing bsa
     */
    void extract(QString bsaPath, const bool& deleteBackup);
    /*!
     * \brief Will create a BSA containing all the files given as argument
     * \param bsa The BSA to create
     */
    void create(Bsa bsa);

    /*!
     * \brief Packs all the loose files in the directory into BSAs
     * \param folderPath The folder to process
     */
    void packAll(const QString &folderPath);

private:
    /*!
     * \brief Will add .bak to the bsa name. If a bak file already exist, their sizes are compared. If the size is the same, the current bsa is removed. Otherwise, the bak file
     * is also renamed.
     * \param bsaPath The BSA to backup
     * \return a QString containing the name of the backup-ed bsa
     */
    QString backup(const QString& bsaPath) const;
    /*!
     * \brief Checks if the file is present in the list filesToNotPack
     * \param filepath The file to check
     * \return a bool indicating the state of the file. True if is ignored, false otherwise
     */
    bool isIgnoredFile(const QString& filepath);
    /*!
     * \brief A list containing the files present in filesToNotPack.txt. If a filename contains a member of this list, it won't be added to the BSA.
     */
    QStringList filesToNotPack;
    /*!
     * \brief Checks if the selected file can be compressed (i.e. if it isn't a sound file)
     * \param filename The file to check
     * \return a bool indicating the state of the file. True if it can be compressed, false otherwise
     */
    bool canBeCompressedFile(const QString& filename);
};
