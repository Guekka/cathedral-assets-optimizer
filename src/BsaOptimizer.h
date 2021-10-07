/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "FilesystemOperations.h"
#include "Profiles.h"
#include "TexturesOptimizer.h"
#include "pch.h"

class OptionsCAO;

/*!
 * \brief Manages BSA : extract and create them
 */
class BSAOptimizer final : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(BsaOptimizer)

public:
    /*!
   * \brief Default constructor
   */
    BSAOptimizer();
    /*!
   * \brief Extracts a BSA
   * \param bsaPath The path of the BSA to extract
   * \param deleteBackup Deletes the backup the existing bsa
   */
    void extract(QString bsaPath, const bool deleteBackup) const;
    /*!
   * \brief Creates a BSA containing all the files given as argument
   * \param bsa The BSA to create
   */
    int create(btu::bsa::ArchiveData &bsa, bool allowCompression, bool deleteSource) const;

    /*!
   * \brief Packs all the loose files in the directory into BSAs
   * \param folderPath The folder to process
   */
    void packAll(const QString &folderPath, const OptionsCAO &options) const;

private:
    /*!
   * \brief Adds .bak to the bsa name. If a bak file already exist, their sizes are compared. If the size is the same, the current bsa is removed. Otherwise, the bak file
   * is also renamed.
   * \param bsaPath The BSA to backup
   * \return a QString containing the name of the backup-ed bsa
   */
    QString backup(const QString &bsaPath) const;
    /*!
   * \brief Checks if the file is present in the list filesToNotPack
   * \return a bool indicating the state of the file. True if is allowed, false otherwise
   */
    bool isAllowedFile(const btu::common::Path &dir,
                       const std::filesystem::directory_entry &fileinfo) const;
    /*!
   * \brief A list containing the files present in filesToNotPack.txt. If a filename contains a member of this list, it won't be added to the BSA.
   */
    std::vector<btu::bsa::fs::path::string_type> filesToNotPack;
};
