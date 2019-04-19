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
    void bsaExtract(QString bsaPath, const bool& makeBackup, const bool& keepFileInBsaFolder);
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
    QString backupOldBsa(const QString& bsaPath) const;
};

#endif // BSAOPTIMIZER_H
