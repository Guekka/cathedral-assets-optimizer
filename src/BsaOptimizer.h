#ifndef BSAOPTIMIZER_H
#define BSAOPTIMIZER_H

#include "pch.h"
#include "QLogger.h"
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
     */
    void bsaExtract(const QString& bsaPath);
    /*!
     * \brief Will create a BSA
     * \param bsaFolderPath The path of the folder to pack into a BSA
     */
    void bsaCreate(const QString& bsaFolderPath);

    bool bBsaPackLooseFiles{};
    bool bBsaDeleteBackup{};
    bool bBsaSplitAssets{};

private:
    /*!
     * \brief Will move the files to the root folder
     * \param bsaFolderPath The path of the folder to move files from
     * \return A bool that indicates the sucess of the operation
     */
    bool moveFilesFromBsaFolderToRootFolder(const QString& bsaFolderPath);
};

#endif // BSAOPTIMIZER_H
