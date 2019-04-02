#include "BsaOptimizer.h"

BsaOptimizer::BsaOptimizer() {}


void BsaOptimizer::bsaExtract(const QString& bsaPath, bool makeBackup) //Extracts all BSA in modPath
{
    QDir directory(bsaPath.left(bsaPath.lastIndexOf("/")));
    QStringList files(directory.entryList());
    QString bsaFolder = directory.filePath(bsaPath + ".extracted");

    directory.mkdir(bsaFolder);

    QStringList bsarchArgs;

    if(!makeBackup)
        bsarchArgs << "unpack" << bsaPath << bsaFolder ;
    else
        bsarchArgs << "unpack" << backupOldBsa(bsaPath) << bsaFolder ;

    QProcess bsarch;
    bsarch.start(QCoreApplication::applicationDirPath() + "/resources/bsarch.exe", bsarchArgs);
    bsarch.waitForFinished(-1);

    QLogger::QLog_Debug("BsaOptimizer", "BSArch Args :" + bsarchArgs.join(" ") + "\nBSA folder :" + bsaFolder + "\nBSA Name :" + bsaPath);

    if(bsarch.readAllStandardOutput().contains("Done"))
    {
        if(!moveFilesFromBsaFolderToRootFolder(bsaFolder))
            QLogger::QLog_Error("BsaOptimizer", tr("An error occured during the extraction. The BSA was correctly extracted, but the files were left inside a subdirectory."));

        else if(!makeBackup)
            QFile::remove(bsaPath);
    }
    else
        QLogger::QLog_Error("BsaOptimizer", tr("An error occured during the extraction. Please extract it manually. The BSA was not deleted."));
}


void BsaOptimizer::bsaCreate(const QString &bsaFolderPath) //Once all the optimizations are done, create a new BSA
{
    QDir bsaDir(bsaFolderPath);
    QStringList bsaSubDirs(bsaDir.entryList(QDir::Dirs));

    //Detecting if BSA will contain sounds, since compressing BSA breaks sounds. Same for strings, Wrye Bash complains

    bool doNotCompress = false;
    for (int j = 0; j < bsaSubDirs.size(); ++j)
    {
        if(bsaSubDirs.at(j).toLower() == "sound" || bsaSubDirs.at(j).toLower() == "music" || bsaSubDirs.at(j).toLower() == "strings")
            doNotCompress=true;
    }

    //Checking if it a textures BSA

    QString bsaName = bsaFolderPath.chopped(10); //Removing ".extracted"
    QStringList bsarchArgs {"pack", bsaFolderPath, bsaName, "-sse", "-share"};

    if (!doNotCompress) //Compressing BSA breaks sounds
        bsarchArgs << "-z";

    QProcess bsarch;

    if(!QFile(bsaName).exists())
    {
        bsarch.start(QCoreApplication::applicationDirPath() + "/resources/bsarch.exe", bsarchArgs);
        bsarch.waitForFinished(-1);
    }
    else
    {
        QLogger::QLog_Error("BsaOptimizer", tr("Cannot pack existing loose files: a BSA already exists."));
        moveFilesFromBsaFolderToRootFolder(bsaFolderPath);
    }

    QLogger::QLog_Debug("BsaOptimizer", "BSArch Args :" + bsarchArgs.join(" ") + "\nBSA folder :" + bsaFolderPath + "\nBsaName : " + bsaName + "\nBSAsize: " + QString::number(QFile(bsaName).size()));

    if(bsarch.readAllStandardOutput().contains("Done"))
    {
        if(QFile(bsaName).size() < 2362232012)
        {
            QLogger::QLog_Note("BsaOptimizer", tr("BSA successfully compressed: ") + bsaName);
            bsaDir.setPath(bsaFolderPath);
            bsaDir.removeRecursively();
        }
        else if(QFile(bsaName).size() < 2469606195)
        {
            QLogger::QLog_Warning("BsaOptimizer", tr("The BSA is nearly over its maximum size. It still should work."));
            bsaDir.setPath(bsaFolderPath);
            bsaDir.removeRecursively();
        }
        else
        {
            QLogger::QLog_Error("BsaOptimizer", tr("The BSA was not compressed: it is over 2.2gb: ") + bsaName);
            moveFilesFromBsaFolderToRootFolder(bsaFolderPath);
            QFile::remove(bsaName);
            if(QFile(bsaName.chopped(3) + "esp").size() == QFile(QCoreApplication::applicationDirPath() + "/resources/BlankSSEPlugin.esp").size())
                QFile::remove(QDir(bsaFolderPath).filePath(bsaName.chopped(3) + "esp"));
        }
    }
}


bool BsaOptimizer::moveFilesFromBsaFolderToRootFolder(const QString &bsaFolderPath)
{
    QString rootFolderPath(bsaFolderPath.left(bsaFolderPath.lastIndexOf("/")));

    try
    {
        FilesystemOperations::moveFiles(bsaFolderPath, rootFolderPath, false);
    }
    catch(const QString& e)
    {
        QLogger::QLog_Error("BsaOptimizer", e);
        return  false;
    }
    return true;
}


QString BsaOptimizer::backupOldBsa(const QString& bsaPath)
{
    QFile bsaBackupFile(bsaPath + ".bak");
    QFile bsaFile(bsaPath);

    if(!bsaBackupFile.exists())
        QFile::rename(bsaPath, bsaBackupFile.fileName());
    else
    {
        if(bsaFile.size() == bsaBackupFile.size())
            QFile::remove(bsaBackupFile.fileName());
        else
            QFile::rename(bsaBackupFile.fileName(), bsaBackupFile.fileName() + ".bak");
    }

    QFile::rename(bsaPath, bsaBackupFile.fileName());

    return bsaBackupFile.fileName();
}
