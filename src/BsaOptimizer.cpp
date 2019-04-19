#include "BsaOptimizer.h"

BsaOptimizer::BsaOptimizer() {}


void BsaOptimizer::bsaExtract(QString bsaPath, const bool &makeBackup, const bool &keepFileInBsaFolder)
{
    QDir directory(QFileInfo(bsaPath).path());
    QString bsaFolder = directory.filePath(bsaPath + ".extracted");

    bsaPath = backupOldBsa(bsaPath);

    BSArchiveAuto archive;
    try {
        archive.open(bsaPath);
        archive.extractAll(bsaFolder);
    } catch (std::exception e) {
        QLogger::QLog_Error("BsaOptimizer", e.what());
        QLogger::QLog_Error("BsaOptimizer", tr("An error occured during the extraction of: ") + bsaPath + "\n" + tr("Please extract it manually. The BSA was not deleted."));
        archive.close();
        return;
    }

    if(!keepFileInBsaFolder)
        if(!moveFilesFromBsaFolderToRootFolder(bsaFolder))
            QLogger::QLog_Error("BsaOptimizer", tr("An error occured during the extraction. The BSA was correctly extracted, but the files were left inside a subdirectory."));

    if(!makeBackup)
        QFile::remove(bsaPath);

    QLogger::QLog_Info("BsaOptimizer", tr("BSA successfully extracted: ") + bsaPath);
}


void BsaOptimizer::bsaCreate(const QString &bsaFolderPath) //Once all the optimizations are done, create a new BSA
{
    QDir bsaDir(bsaFolderPath);
    QStringList bsaSubDirs(bsaDir.entryList(QDir::Dirs));

    //Checking if a bsa already exists

    QString bsaName = QString(bsaFolderPath).remove(".extracted");
    if(QFile(bsaName).exists())
    {
        QLogger::QLog_Error("BsaOptimizer", tr("Cannot pack existing loose files: a BSA already exists."));
        return;
    }

    //Detecting if BSA will contain sounds, since compressing BSA breaks sounds. Same for strings, Wrye Bash complains

    bool doNotCompress = false;
    for (int j = 0; j < bsaSubDirs.size(); ++j)
    {
        if(bsaSubDirs.at(j).toLower() == "sound" || bsaSubDirs.at(j).toLower() == "music" || bsaSubDirs.at(j).toLower() == "strings")
            doNotCompress=true;
    }

    BSArchiveAuto archive;
    archive.setShareData(true);

    if(!doNotCompress) //Compressing BSA breaks sounds and strings
        archive.setCompressed(true);

    QStringList files;
    QDirIterator it(bsaFolderPath, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        QFileInfo file = it.next();

        if(!file.isDir())
        {
            try {
                archive.addFileFromDisk(bsaFolderPath, file.filePath());
            } catch (std::exception e) {
                QLogger::QLog_Error("BsaOptimizer", e.what());
                QLogger::QLog_Error("BsaOptimizer", "Cancelling packing of: " + bsaFolderPath);
                archive.close();
                return;
            }
        }
    }

    archive.create(bsaName, baSSE);
    archive.save();
    archive.close();

    QLogger::QLog_Debug("BsaOptimizer", "\nBSA folder :" + bsaFolderPath + "\nBsaName : " + bsaName + "\nBSAsize: "
                        + QString::number(QFile(bsaName).size()));

    if(QFile(bsaName).size() < LONG_MAX)
    {
        QLogger::QLog_Note("BsaOptimizer", tr("BSA successfully compressed: ") + bsaName);
        bsaDir.setPath(bsaFolderPath);
        bsaDir.removeRecursively();
    }
    else
    {
        QLogger::QLog_Error("BsaOptimizer", tr("The BSA was not compressed: it is over 2.15gb: ") + bsaName);
        moveFilesFromBsaFolderToRootFolder(bsaFolderPath);
        QFile::remove(bsaName);
        if(QFile(bsaName.chopped(3) + "esp").size() == QFile(QCoreApplication::applicationDirPath() + "/resources/BlankSSEPlugin.esp").size())
            QFile::remove(QDir(bsaFolderPath).filePath(bsaName.chopped(3) + "esp"));
    }
}


bool BsaOptimizer::moveFilesFromBsaFolderToRootFolder(const QString &bsaFolderPath)
{
    QString rootFolderPath = QFileInfo(bsaFolderPath).path();
    return FilesystemOperations::moveFiles(bsaFolderPath, rootFolderPath, false);
}


QString BsaOptimizer::backupOldBsa(const QString& bsaPath) const
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
