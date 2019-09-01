/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "FilesystemOperations.h"
#include "Profiles.h"
#include "TexturesOptimizer.h"
#include "pch.h"

enum BsaType
{
    TexturesBsa = 0,
    StandardBsa,
    TexturesAndStandardBsa
};

/*!
 * \brief Manages BSA : extract and create them
 */
class BsaOptimizer final : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(BsaOptimizer)

public:
    /*! 
     * \brief Used for several internal operations
     */
    struct Bsa
    {
        QString path;
        qint64 filesSize = 0;
        QStringList files;
        double maxSize = LONG_MAX;
        BsaType type = StandardBsa;
        bsa_archive_type_t format;
    };

    /*!
   * \brief Default constructor
   */
    BsaOptimizer();
    /*!
   * \brief Extracts a BSA
   * \param bsaPath The path of the BSA to extract
   * \param deleteBackup Deletes the backup the existing bsa
   */
    void extract(QString bsaPath, const bool &deleteBackup) const;
    /*!
   * \brief Creates a BSA containing all the files given as argument
   * \param bsa The BSA to create
   */
    int create(Bsa &bsa) const;

    /*!
   * \brief Packs all the loose files in the directory into BSAs
   * \param folderPath The folder to process
   */
    void packAll(const QString &folderPath) const;

    static void DDSCallback(bsa_archive_t archive, const wchar_t *file_path, bsa_dds_info_t *dds_info, void *context);

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
   * \param filepath The file to check
   * \return a bool indicating the state of the file. True if is ignored, false otherwise
   */
    bool isIgnoredFile(const QString &filepath) const;
    /*!
   * \brief A list containing the files present in filesToNotPack.txt. If a filename contains a member of this list, it won't be added to the BSA.
   */
    QStringList filesToNotPack;
    /*!
   * \brief Checks if the selected file can be compressed (i.e. if it isn't a sound file)
   * \param filename The file to check
   * \return a bool indicating the state of the file. True if it can be compressed, false otherwise
   */
    static bool canBeCompressedFile(const QString &filename);
};

namespace plog
{
    inline Record &operator<<(Record &record, const BsaOptimizer::Bsa &bsa)
    {
        return record << "BSA Structure:\nPath: " + bsa.path
                             + " \nUncompressed files size: " + QString::number(bsa.filesSize / GigaByte) + "Gb"
                             + "\nmaxSize: " + QString::number(bsa.maxSize / GigaByte) + "Gb" + "\nType: "
                      << bsa.type << "\nFormat: " << bsa.format;
    }
} // namespace plog
