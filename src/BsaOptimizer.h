/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#include "QLogger/QLogger.h"
#include "FilesystemOperations.h"

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
     * \param deleteBsaBackup Backup the existing bsa
     * \param keepFileInBsaFolder Whether files will be moved to mod root or not
     */
    void bsaExtract(const QString& bsaPath, const bool& makeBackup, const bool& keepFileInBsaFolder);
    /*!
     * \brief Will create a BSA
     * \param bsaFolderPath The path of the folder to pack into a BSA
     */
    void bsaCreate(const QString& bsaFolderPath);


private:
    /*!
     * \brief Will move the files to the root folder
     * \param bsaFolderPath The path of the folder to move files from
     * \return A bool that indicates the sucess of the operation
     */
    bool moveFilesFromBsaFolderToRootFolder(const QString& bsaFolderPath);
    /*!
     * \brief Will add .bak to the bsa name. If a bak file already exist, their sizes are compared. If the size is the same, the current bsa is removed. Otherwise, the bak file
     * is also renamed.
     * \param bsaPath The BSA to backup
     * \return a QString containing the name of the backup-ed bsa
     */
    QString backupOldBsa(const QString& bsaPath);
};
