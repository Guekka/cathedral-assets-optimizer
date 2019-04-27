#ifndef BSAOPTIMIZER_H
#define BSAOPTIMIZER_H

#include "pch_core.h"
#include "QLogger.h"
#include "BSArchiveAuto.h"
#include "FilesystemOperations.h"

/*!
 * \brief The BsaOptimizer class
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
    void extract(QString bsaPath, const bool& makeBackup);

    /*!
     * \brief Will create a BSA containing all the files given as argument
     * \param bsaPath The path of the bsa to create. It has to be in the folder containing the assets
     * \param files The files to add to the BSA
     */
    void create(const QString &bsaPath, const QStringList &files);

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

#endif // BSAOPTIMIZER_H
