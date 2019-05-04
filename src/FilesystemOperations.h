#ifndef FILESYSTEMOPERATIONS_H
#define FILESYSTEMOPERATIONS_H

#include "pch.h"
#include "QLogger/QLogger.h"
#include "PluginsOperations.h"

/*!
 * \brief Manages filesystem operations : move files, calculate folder size...
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
     */
    void prepareBsas(const QString& folderPath);
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
    static QPair<qint64, qint64> assetsSize(const QString& path);
    /*!
         * \brief Delete empty directories in the given directory
         * \param folderPath The path of the folder where empty dirs will be deleted
         */
    static void deleteEmptyDirectories(const QString& folderPath);
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

    QStringList filesToNotPack;
};

#endif // FILESYSTEMOPERATIONS_H
