#include "FilesystemOperations.h"

const QStringList texturesAssets {"png", "dds"};
const QStringList otherAssets {"nif", "seq", "pex", "psc", "lod", "fuz", "waw", "xwm", "swf", "hkx", "wav", "tri", "btr", "bto", "btt", "lip", "txt"};
const QStringList allAssets = texturesAssets + otherAssets;


FilesystemOperations::FilesystemOperations()
{
    //Reading filesToNotPack to add them to the list.
    //Done in the constructor since the file won't change at runtime.

    QFile filesToNotPackFile(QCoreApplication::applicationDirPath() + "/resources/FilesToNotPack.txt");
    if(filesToNotPackFile.open(QIODevice::ReadOnly))
    {
        QTextStream ts(&filesToNotPackFile);
        while (!ts.atEnd())
        {
            QString readLine = ts.readLine();
            if(readLine.left(1) != "#" && !readLine.isEmpty())
                filesToNotPack << readLine;
        }
    }
    else
        QLogger::QLog_Warning("FilesystemOperations", tr("FilesToNotPack.txt not found. Animations will be packed, preventing them from being detected by FNIS and Nemesis."));
}


void FilesystemOperations::prepareBsas(const QString &folderPath, const bool &splitAssets) //Split assets between several folders
{
    QLogger::QLog_Trace("FilesystemOperations", "Entering " + QString(__FUNCTION__) + " function");

    QDir directory(folderPath);

    QStringList bsaList;
    QStringList texturesBsaList;
    QStringList dirs(directory.entryList(QDir::Dirs));

    QLogger::QLog_Trace("FilesystemOperations", "Listing all BSA folders and moving files to modpath root directory");

    for (int i = 0; i < dirs.size(); ++i)
    {
        if(dirs.at(i).right(13) == "bsa.extracted" && dirs.at(i).contains("- Textures", Qt::CaseInsensitive))
        {
            texturesBsaList << directory.filePath(dirs.at(i));
            moveFiles(directory.filePath(dirs.at(i)), directory.path(), false);
        }
        else if(dirs.at(i).right(13) == "bsa.extracted")
        {
            bsaList << directory.filePath(dirs.at(i));
            moveFiles(directory.filePath(dirs.at(i)), directory.path(), false);
        }
    }

    QString espName = PluginsOperations::findPlugin(folderPath).remove(".esp");
    QString bsaName;

    if(splitAssets)
    {
        QLogger::QLog_Trace("FilesystemOperations", "Creating enough folders to contain all the files");

        QPair<qint64, qint64> size = assetsSize(directory.path());
        int i = 0;

        while(texturesBsaList.size() < qCeil(size.first/2547483647.0))
        {
            if(i == 0)
                bsaName = espName + " - Textures.bsa.extracted";
            else
                bsaName = espName + QString::number(i) + " - Textures.bsa.extracted";

            texturesBsaList << bsaName;
            texturesBsaList.removeDuplicates();
            ++i;
        }

        i = 0;
        while(bsaList.size() < qCeil(size.second/2107483647.0))
        {
            if(i == 0)
                bsaName = espName + ".bsa.extracted";
            else
                bsaName = espName + QString::number(i) + ".bsa.extracted";

            bsaList << bsaName ;
            bsaList.removeDuplicates();
            ++i;
        }
    }

    else //If assets splitting is disabled, use only one bsa folder and one textures bsa folder
    {
        if(texturesBsaList.isEmpty())
            texturesBsaList << espName + " - Textures.bsa.extracted";
        if(bsaList.isEmpty())
            bsaList << espName + ".bsa.extracted";
    }

    moveAssets(folderPath, bsaList, texturesBsaList);

    QLogger::QLog_Trace("FilesystemOperations", "Total: " + QString::number(bsaList.size()) + " bsa folders:\n" + bsaList.join("\n") + "\n"
                        + QString::number(texturesBsaList.size()) + " textures bsa folders:\n" + texturesBsaList.join("\n"));

    QLogger::QLog_Trace("FilesystemOperations", "Exiting splitAssets function");
}

void FilesystemOperations::moveFiles(const QString& source, const QString& destination, bool overwriteExisting)
{
    QDir sourceDir(source);
    QDir destinationDir(destination);
    QDirIterator it(source, QDirIterator::Subdirectories);

    QLogger::QLog_Trace("FilesystemOperations", "Entering " + QString(__FUNCTION__) + " function");
    QLogger::QLog_Debug("FilesystemOperations", "dest folder: " + destination + "\nsource folder: " + source);

    QStringList oldFiles;

    while (it.hasNext())
    {
        it.next();
        if (!QFileInfo(it.filePath()).isDir()) //Skipping all directories and avoiding to copy from the destination folder
            oldFiles << QDir::cleanPath(destinationDir.relativeFilePath(it.filePath()));
    }

    oldFiles.removeDuplicates();

    for (int i = 0 ; i < oldFiles.size() ; ++i)
    {
        QString relativeFilename = QDir::cleanPath(sourceDir.relativeFilePath(oldFiles.at(i)));
        QString newFileRelativeFilename = destinationDir.relativeFilePath(QDir::cleanPath(destination) + QDir::separator() + relativeFilename);

        if(newFileRelativeFilename.size() >= 255)
        {
            QLogger::QLog_Error("FilesystemOperations", tr("An error occurred while moving files. Try reducing path size (260 characters is the maximum)"));
            return;
        }

        //removing the duplicate files from new folder (if overwriteExisting) or from old folder (if !overwriteExisting)

        destinationDir.mkpath(QFileInfo(newFileRelativeFilename).path());

        if(overwriteExisting)
            destinationDir.remove(newFileRelativeFilename);

        destinationDir.rename(oldFiles.at(i), newFileRelativeFilename);

        if(!overwriteExisting)
            destinationDir.remove(oldFiles.at(i));
    }
    QLogger::QLog_Trace("FilesystemOperations", "Exiting moveFiles function");
}


void FilesystemOperations::moveAssets(const QString &path, const QStringList &bsaList, const QStringList &texturesBsaList)
{
    QLogger::QLog_Trace("FilesystemOperations", "Splitting files between bsa folders");

    int k = 0;
    int j = 0;

    deleteEmptyDirectories(path);
    QDir directory(path);
    QDirIterator it(directory, QDirIterator::Subdirectories);
    QStringList oldFiles;

    while(it.hasNext())
    {
        it.next();

        //Skipping all directories and avoiding unnecessary files
        QFileInfo info(it.filePath());
        bool isDir = info.isDir();
        bool hasAssets = allAssets.contains(it.fileName().right(3), Qt::CaseInsensitive);
        bool canBePacked = true;

        for(int i = 0 ; i < filesToNotPack.size() ; ++i)
        {
            if(it.filePath().contains(filesToNotPack.at(i)))
                canBePacked = false;
        }

        if(!isDir && hasAssets && canBePacked)
        {
            QLogger::QLog_Debug("FilesystemOperations", "all check are true");
            oldFiles << directory.relativeFilePath(it.filePath());
        }
    }

    oldFiles.removeDuplicates();

    QLogger::QLog_Debug("FilesystemOperations", "oldFiles.size: " + QString::number(oldFiles.size()));

    for (int i = 0 ; i < oldFiles.size() ; ++i)
    {
        QString newFile;
        if(otherAssets.contains(oldFiles.at(i).right(3), Qt::CaseInsensitive))
        {
            ++k;
            if(k >= bsaList.size() || k < 0)
                k = 0;
            newFile = directory.relativeFilePath(bsaList.at(k) + "/" + oldFiles.at(i));
        }

        else if(texturesAssets.contains(oldFiles.at(i).right(3), Qt::CaseInsensitive))
        {
            ++j;
            if(j >= texturesBsaList.size() || j < 0)
                j = 0;
            newFile = directory.relativeFilePath(texturesBsaList.at(j) + "/" + oldFiles.at(i));
        }

        QLogger::QLog_Debug("FilesystemOperations", "\nOld file: " + oldFiles.at(i)
                            + "\nNew file: " + newFile);

        if(newFile.size() >= 255)
        {
            QLogger::QLog_Error("FilesystemOperations", tr("An error occurred while moving files. Try reducing path size (260 characters is the maximum)"));
            return;
        }

        directory.mkpath(QFileInfo(newFile).path());
        directory.rename(oldFiles.at(i), newFile);
    }
}


void FilesystemOperations::deleteEmptyDirectories(const QString &folderPath)
{
    QDirIterator dirIt(folderPath, QDirIterator::Subdirectories);
    QMap<int, QStringList> dirs;

    while(dirIt.hasNext())
    {
        QString path =  QDir::cleanPath(dirIt.next());
        int size = path.size();
        if(!dirs[size].contains(path))
            dirs[size].append(path);
    }

    QDir dir(folderPath);
    QMapIterator<int, QStringList> i (dirs);

    i.toBack();
    while (i.hasPrevious())
    {
        i.previous();
        for(int j = 0; j < i.value().size(); ++j)
        {
            dir.rmpath(i.value().at(j));
        }
    }
}


QPair <qint64, qint64> FilesystemOperations::assetsSize(const QString& path) // Return textures size and other assets size in a directory
{
    QPair <qint64, qint64> size;
    //First will be textures, second will be other assets

    QDirIterator it(path, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        QFile currentFile(it.next());

        if(texturesAssets.contains(it.fileName().right(3), Qt::CaseInsensitive))
            size.first += currentFile.size();
        else if(otherAssets.contains(currentFile.fileName().right(3), Qt::CaseInsensitive))
            size.second += currentFile.size();
    }
    return size;
}



QString FilesystemOperations::findSkyrimDirectory() //Find Skyrim directory using the registry key
{
    QSettings SkyrimReg(R"(HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Bethesda Softworks\Skyrim Special Edition)", QSettings::NativeFormat);
    QString SkyrimDir = QDir::cleanPath(SkyrimReg.value("Installed Path").toString());
    return SkyrimDir;
}

