#include "BSArchiveAuto.h"
#include "QLibbsarch.h"

BSArchiveAuto::BSArchiveAuto(const QString& rootDirectory) : rootDirectory(QDir::toNativeSeparators(QDir::cleanPath(rootDirectory))) {}


void BSArchiveAuto::create(const QString &archiveName, const bsa_archive_type_e &type)
{
    const wchar_t *path = QStringToWchar( QDir::toNativeSeparators(archiveName) );
    bsa_create_archive(getArchive(), path, type, getEntries());

    QMapIterator<QString, QByteArray> mapItMemory(filesfromMemory);
    QMapIterator<QString, QString> mapItDisk(filesFromDisk);

    while(mapItMemory.hasNext())
    {
        mapItMemory.next();
        BSArchive::addFileFromMemory(mapItMemory.key(), mapItMemory.value());
    }

    for (auto file : filesFromDiskRoot)
        BSArchive::addFileFromDiskRoot(rootDirectory.path(), file);

    while(mapItDisk.hasNext())
    {
        mapItDisk.next();
        BSArchive::addFileFromDisk(mapItDisk.key(), mapItDisk.value());
    }
}


void BSArchiveAuto::addFileFromDiskRoot(const QString &filename)
{
    add(rootDirectory.relativeFilePath(filename));
    filesFromDiskRoot << filename;
}


void BSArchiveAuto::addFileFromDiskRoot(const QStringList& files)
{
    for (auto file : files)
        addFileFromDiskRoot(file);
}

void BSArchiveAuto::addFileFromDisk(const QString &saveAs, const QString &diskPath)
{
    BSArchiveEntries::add(saveAs);
    filesFromDisk.insert(saveAs, diskPath);
}

void BSArchiveAuto::addFileFromDisk(const QMap<QString, QString> &map)
{
    QMapIterator<QString, QString> mapIt(map);

    while(mapIt.hasNext())
    {
        mapIt.next();
        filesFromDisk.insert(mapIt.key(), mapIt.value());
    }
}

void BSArchiveAuto::addFileFromMemory(const QString &filename, const QByteArray &data)
{
    add(filename);
    filesfromMemory.insert(filename, data);
}

void BSArchiveAuto::extractAll(const QString& destinationDirectory, const bool &overwriteExistingFiles)
{
    for (auto file : BSArchive::listFiles())
    {
        QFile currentFile(destinationDirectory + "/" + file);
        rootDirectory.mkpath(destinationDirectory + "/" + QFileInfo(file).path());
        if(currentFile.exists() && overwriteExistingFiles)
        {
            currentFile.remove();
            BSArchive::extract(file, currentFile.fileName());
        }
        else if (!currentFile.exists())
            BSArchive::extract(file, currentFile.fileName());
    }
}

void BSArchiveAuto::reset()
{
    BSArchive::reset();
    BSArchiveEntries::reset();

}
