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


void FilesystemOperations::prepareBsas(const QString &folderPath, const bool &splitAssets, const bool &mergeLoose)
{
    QLogger::QLog_Trace("FilesystemOperations", "Entering " + QString(__FUNCTION__) + " function");

    QDir directory(folderPath);

    QStringList foundBsa;
    QStringList foundTexturesBsa;
    QStringList dirs(directory.entryList(QDir::Dirs));

    QLogger::QLog_Trace("FilesystemOperations", "Listing all BSA folders and moving files to modpath root directory");

    for (int i = 0; i < dirs.size(); ++i)
    {
        if(dirs.at(i).right(13) == "bsa.extracted" && dirs.at(i).contains("- Textures", Qt::CaseInsensitive))
        {
            foundTexturesBsa << directory.filePath(dirs.at(i));
            if(mergeLoose)
                moveFiles(directory.filePath(dirs.at(i)), directory.path(), false);
        }
        else if(dirs.at(i).right(13) == "bsa.extracted")
        {
            foundBsa << directory.filePath(dirs.at(i));
            if(mergeLoose)
                moveFiles(directory.filePath(dirs.at(i)), directory.path(), false);
        }
    }

    QString espName = PluginsOperations::findPlugin(folderPath).remove(".esp");
    QString bsaName;

    QStringList addedBsa;
    QStringList addedTexturesBsa;

    if(splitAssets)
    {
        QLogger::QLog_Trace("FilesystemOperations", "Creating enough folders to contain all the files");

        QPair<qint64, qint64> size = assetsSize(directory.path());
        int i = 0;

        while(foundTexturesBsa.size() < qCeil(size.first/2547483647.0))
        {
            if(i == 0)
                bsaName = espName + " - Textures.bsa.extracted";
            else
                bsaName = espName + QString::number(i) + " - Textures.bsa.extracted";

            if(!foundTexturesBsa.contains(bsaName))
                addedTexturesBsa << bsaName;
            addedTexturesBsa.removeDuplicates();
            ++i;
        }

        i = 0;
        while(foundBsa.size() < qCeil(size.second/2107483647.0))
        {
            if(i == 0)
                bsaName = espName + ".bsa.extracted";
            else
                bsaName = espName + QString::number(i) + ".bsa.extracted";

            if(!foundBsa.contains(bsaName))
                addedBsa << bsaName ;
            addedBsa.removeDuplicates();
            ++i;
        }
    }

    else //If assets splitting is disabled, use only one bsa folder and one textures bsa folder
    {
        if(foundTexturesBsa.isEmpty())
            addedTexturesBsa << espName + " - Textures.bsa.extracted";
        if(foundBsa.isEmpty())
            addedBsa << espName + ".bsa.extracted";
    }

    if(mergeLoose)
    {
        foundBsa += addedBsa;
        foundTexturesBsa += addedTexturesBsa;
    }

    //TODO work on new bsa system

    system(qPrintable("cd /d \"" + folderPath + R"(" && for /f "delims=" %d in ('dir /s /b /ad ^| sort /r') do rd "%d" >nul 2>nul)"));

    moveAssets(folderPath, foundBsa, foundTexturesBsa);

    QLogger::QLog_Trace("FilesystemOperations", "Total: " + QString::number(foundBsa.size()) + " bsa folders:\n" + foundBsa.join("\n") + "\n"
                        + QString::number(foundTexturesBsa.size()) + " textures bsa folders:\n" + foundTexturesBsa.join("\n"));

    system(qPrintable("cd /d \"" + folderPath + R"(" && for /f "delims=" %d in ('dir /s /b /ad ^| sort /r') do rd "%d" >nul 2>nul)"));

    QLogger::QLog_Trace("FilesystemOperations", "Exiting splitAssets function");
}


void FilesystemOperations::moveFiles(const QString& source, const QString& destination, bool overwriteExisting)
{
    QDir sourceDir(source);
    QDir destinationDir(destination);
    QDirIterator it(source, QDirIterator::Subdirectories);

    QLogger::QLog_Trace("FilesystemOperations", "Entering " + QString(__FUNCTION__) + " function");
    QLogger::QLog_Debug("FilesystemOperations", "dest folder: " + destination + "\nsource folder: " + source);

    sourceDir.mkdir(destination);

    while (it.hasNext())
    {
        it.next();
        if (!QFileInfo(it.filePath()).isDir() && it.path() != destination) //Skipping all directories and avoiding to copy from the destination folder
        {

            QString relativeFilename = QDir::cleanPath(sourceDir.relativeFilePath(it.filePath()));
            QFile oldFile(it.filePath());
            QFile newFile(QDir::cleanPath(destination) + QDir::separator() + relativeFilename);

            QString newFileRelativeFilename = destinationDir.relativeFilePath(newFile.fileName());
            QString oldFileRelativeFilename = destinationDir.relativeFilePath(oldFile.fileName());

            if(newFileRelativeFilename.size() >= 255)
                throw tr("An error occurred while moving files. Try reducing path size (260 characters is the maximum)");

            //removing the duplicate files from new folder (if overwriteExisting) or from old folder (if !overwriteExisting)

            destinationDir.mkpath(QFileInfo(newFileRelativeFilename).path());

            if(overwriteExisting)
                destinationDir.remove(newFileRelativeFilename);

            destinationDir.rename(oldFileRelativeFilename, newFileRelativeFilename);

            if(!overwriteExisting)
                destinationDir.remove(oldFileRelativeFilename);
        }
    }
    QLogger::QLog_Trace("FilesystemOperations", "Exiting moveFiles function");
}


void FilesystemOperations::moveAssets(const QString &path, const QStringList &bsaList, const QStringList &texturesBsaList)
{
    QLogger::QLog_Trace("FilesystemOperations", "Splitting files between bsa folders");

    int k = 0;
    int j = 0;

    QDir directory(path);
    QDirIterator it(directory, QDirIterator::Subdirectories);

    while(it.hasNext())
    {
        it.next();
        //Skipping all directories and avoiding unnecessary files
        bool isDir = QFileInfo(it.filePath()).isDir();
        bool hasAssets = allAssets.contains(it.fileName().right(3), Qt::CaseInsensitive);
        bool canBePacked = true;

        for(int i = 0 ; i < filesToNotPack.size() ; ++i)
        {
            if(it.filePath().contains(filesToNotPack.at(i)))
            {
                //qDebug() << it.filePath() << filesToNotPack.at(i);
                canBePacked = false;
            }
        }

        if(!isDir && hasAssets && canBePacked)
        {
            QString relativeFilename = directory.relativeFilePath(it.filePath());
            QString newAssetRelativeFilename;

            if(otherAssets.contains(it.fileName().right(3), Qt::CaseInsensitive))
            {
                ++k;
                if(k >= bsaList.size() || k < 0)
                    k = 0;
                newAssetRelativeFilename = directory.relativeFilePath(bsaList.at(k) + "/" + relativeFilename);
            }

            else if(texturesAssets.contains(it.fileName().right(3), Qt::CaseInsensitive))
            {
                ++j;
                if(j >= texturesBsaList.size() || j < 0)
                    j = 0;

                newAssetRelativeFilename = directory.relativeFilePath(texturesBsaList.at(j) + "/" + relativeFilename);
            }

            QString oldAssetRelativeFilename = directory.relativeFilePath(it.filePath());

            //removing the duplicate assets and checking for path size

            if(newAssetRelativeFilename.size() >= 255) //Max path size for Windows
                throw tr("The filepath is more than 260 characters long. Please reduce it.");

            directory.mkpath(QFileInfo(newAssetRelativeFilename).path());
            directory.rename(oldAssetRelativeFilename, newAssetRelativeFilename);
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


void FilesystemOperations::deleteEmptyDirectories(const QString &folderPath)
{
    QDir dir(folderPath);
    QDirIterator it (folderPath);

    while(it.hasNext())
    {
        dir.rmpath(it.next());
    }
}


QString FilesystemOperations::findSkyrimDirectory() //Find Skyrim directory using the registry key
{
    QSettings SkyrimReg(R"(HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Bethesda Softworks\Skyrim Special Edition)", QSettings::NativeFormat);
    QString SkyrimDir = QDir::cleanPath(SkyrimReg.value("Installed Path").toString());
    return SkyrimDir;
}

