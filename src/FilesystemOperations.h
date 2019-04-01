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
     * \brief Will split assets into several folders. What file is an asset is defined by an hardcoded list.
     * \param folderPath The path of the folder where assets will be splitted
     */
    static void splitAssets(const QString& folderPath);
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
     * \brief Will calculate the size of all assets in the given path
     * \param path The path of the directory to scan
     * \return  A QPair containing two qint64. The first will contain the textures size, the second will contain the other assets size.
     */
    static QPair<qint64, qint64> assetsSize(const QString& path);
};

#endif // FILESYSTEMOPERATIONS_H
