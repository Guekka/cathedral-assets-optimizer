/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#include "QLogger/QLogger.h"
#include "BSArchiveAuto.h"
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
     * \param deleteBackup Deletes the backup the existing bsa
     */
    void extract(QString bsaPath, const bool& deleteBackup);
    /*!
     * \brief Will create a BSA containing all the files given as argument
     * \param bsaPath The path of the bsa to create. It has to be in the folder containing the assets
     * \param files The files to add to the BSA
     */
    void create(const QString &bsaPath, QStringList &files);

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
     * \brief A list containing the files present in filesToNotPack.txt. If a filename contains a member of this list, it won't be added to the BSA.
     */
    QStringList filesToNotPack;

};
