#ifndef FILESYSTEMOPERATIONS_H
#define FILESYSTEMOPERATIONS_H

#include "pch.h"
#include "QLogger.h"
#include "PluginsOperations.h"

/*!
 * \brief The FilesystemOperations class
 */

class FilesystemOperations : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(FilesystemOperations)

public:

    /*!
     * \brief Constructor that will read FilesToNotPack.txt and add them to filesToNotPack QStringList
     */
    FilesystemOperations();

    /*!
     * \brief Will prepare for bsa creation.
     * \param folderPath The path of the folder where assets will be processed
     * \param splitAssets Whether the assets will be splitted or not
     */
    void prepareBsas(const QString& folderPath, const bool& splitAssets);
    /*!
     * \brief Will move all files from source folder into destination folder.
     * \param source The source directory
     * \param destination The destination directory
     * \param overwriteExisting If enabled, source files will overwrite destination files
     */
    static void moveFiles(const QString& source, const QString& destination, bool overwriteExisting);
    /*!
     * \brief Will find skyrim directory using the registry key.
     * \return A QString containing the path of the Skyrim directory, or an empty QString if the path is not found
     */
    static QString findSkyrimDirectory();
    /*!
     * \brief Will separate assets into several folders. What file is an asset is defined by an hardcoded list.
     * \param path The folder to process
     * \param bsaList The list of all normal bsa (all assets except textures)
     * \param texturesBsaList The list of all textures bsa
     */
    void moveAssets(const QString& path, const QStringList& bsaList, const QStringList& texturesBsaList);

    /*!
     * \brief Will calculate the size of all assets in the given path
     * \param path The path of the directory to scan
     * \return  A QPair containing two qint64. The first will contain the textures size, the second will contain the other assets size.
     */

    static void deleteEmptyDirectories(const QString &folderPath);

    static QPair<qint64, qint64> assetsSize(const QString& path);

private:
    /*!
     * \brief List of files or folder that won't be packed to BSA
     */
    QStringList filesToNotPack;
};

#endif // FILESYSTEMOPERATIONS_H
