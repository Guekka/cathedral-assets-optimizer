#include "BsaOptimizer.h"

BsaOptimizer::BsaOptimizer() : logManager(QLogger::QLoggerManager::getInstance()) {}


void BsaOptimizer::bsaExtract(const QString& bsaPath) //Extracts all BSA in modPath
{
    QDir directory(bsaPath.left(bsaPath.lastIndexOf("/")));
    QStringList files(directory.entryList());
    QString bsaFolder = directory.filePath(bsaPath + ".extracted");

    directory.mkdir(bsaFolder);

    QStringList bsarchArgs;

    if(bBsaDeleteBackup)
        bsarchArgs << "unpack" << bsaPath << bsaFolder ;
    else
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
        bsarchArgs << "unpack" << bsaBackupFile.fileName() << bsaFolder ;
    }

    QProcess bsarch(this);
    bsarch.start(QCoreApplication::applicationDirPath() + "/resources/bsarch.exe", bsarchArgs);
    bsarch.waitForFinished(-1);

    QLogger::QLog_Debug("BsaOptimizer", "BSArch Args :" + bsarchArgs.join(" ") + "\nBSA folder :" + bsaFolder + "\nBSA Name :" + bsaPath);

    if(bsarch.readAllStandardOutput().contains("Done"))
    {
        if(!moveFilesFromBsaFolderToRootFolder(bsaFolder))
            QLogger::QLog_Error("BsaOptimizer", tr("An error occured during the extraction. The BSA was correctly extracted, but the files were left inside a subdirectory."));

        if(bBsaDeleteBackup)
            QFile::remove(bsaPath);
    }
    else
        throw tr("An error occured during the extraction. Please extract it manually. The BSA was not deleted.");
}


void BsaOptimizer::bsaCreate(const QString &bsaFolderPath) //Once all the optimizations are done, create a new BSA
{
    QDir directory(bsaFolderPath.left(bsaFolderPath.lastIndexOf("/")));
    QStringList dirs(directory.entryList(QDir::Dirs));

    QStringList bsarchArgs;
    QString bsaName;

    QDir bsaDir;
    QStringList bsaSubDirs;
    QProcess bsarch;

    bool doNotCompress=false;

    bsaDir.setPath(bsaFolderPath);
    bsaSubDirs = bsaDir.entryList(QDir::Dirs);

    //Detecting if BSA will contain sounds, since compressing BSA breaks sounds. Same for strings, Wrye Bash complains

    doNotCompress = false;
    for (int j = 0; j < bsaSubDirs.size(); ++j)
    {
        if(bsaSubDirs.at(j).toLower() == "sound" || bsaSubDirs.at(j).toLower() == "music" || bsaSubDirs.at(j).toLower() == "strings")
            doNotCompress=true;
    }

    //Checking if it a textures BSA

    bsaName = bsaFolderPath.chopped(10); //Removing ".extracted"
    bsarchArgs << "pack" << bsaFolderPath << bsaName << "-sse" << "-share";

    if (!doNotCompress) //Compressing BSA breaks sounds
        bsarchArgs << "-z";

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
                QFile::remove(directory.filePath(bsaName.chopped(3) + "esp"));

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
