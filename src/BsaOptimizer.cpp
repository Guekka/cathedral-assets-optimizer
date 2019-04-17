#include "BsaOptimizer.h"

BsaOptimizer::BsaOptimizer() {}


void BsaOptimizer::bsaExtract(const QString &bsaPath, const bool &makeBackup, const bool &keepFileInBsaFolder)
{
    QDir directory(QFileInfo(bsaPath).path());
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
         if(!keepFileInBsaFolder)
            if(!moveFilesFromBsaFolderToRootFolder(bsaFolder))
                QLogger::QLog_Error("BsaOptimizer", tr("An error occured during the extraction. The BSA was correctly extracted, but the files were left inside a subdirectory."));

        if(!makeBackup)
            QFile::remove(bsaPath);

        QLogger::QLog_Info("BsaOptimizer", tr("BSA successfully extracted: ") + bsaPath);
    }
    else
        QLogger::QLog_Error("BsaOptimizer", tr("An error occured during the extraction. Please extract it manually. The BSA was not deleted."));
}


void BsaOptimizer::bsaCreate(const QString &bsaFolderPath) //Once all the optimizations are done, create a new BSA
{
    bsa_archive_t archive = bsa_create();

    bsa_entry_list_t entries = bsa_entry_list_create();
    bsa_entry_list_add(entries, L"test.pfx");

    bsa_create_archive(archive, L"test_write.bsa", baSSE, entries);
    bsa_add_file_from_disk(archive, L"C:\\", L"C:\\test.pfx");
    bsa_save(archive);
    bsa_close(archive);

    bsa_entry_list_free(entries);

    bsa_free(archive);

   /* QDir bsaDir(bsaFolderPath);
    QStringList bsaSubDirs(bsaDir.entryList(QDir::Dirs));

    //Detecting if BSA will contain sounds, since compressing BSA breaks sounds. Same for strings, Wrye Bash complains

    bool doNotCompress = false;
    for (int j = 0; j < bsaSubDirs.size(); ++j)
    {
        if(bsaSubDirs.at(j).toLower() == "sound" || bsaSubDirs.at(j).toLower() == "music" || bsaSubDirs.at(j).toLower() == "strings")
            doNotCompress=true;
    }

    wchar_t *bsaFolderPathWc = new wchar_t[sizeof (bsaFolderPath)];
    bsaFolderPath.toWCharArray(bsaFolderPathWc);

    QString bsaNameQS = QString(bsaFolderPath).remove(".extracted"); //Removing ".extracted"
    wchar_t *bsaName = new wchar_t(sizeof(bsaNameQS));
    bsaNameQS.toWCharArray(bsaName);

    bsa_archive_t archive = bsa_create();

    if(!doNotCompress) //Compressing BSA breaks sounds
      bsa_compress_set(archive, true);

    bsa_entry_list_t entries = bsa_entry_list_create();

    QDirIterator it(bsaFolderPath, QDirIterator::Subdirectories);
    QStringList files;
    while (it.hasNext())
    {
      files << it.next().replace("/", "\\");
    }

    qDebug() << *bsaFolderPathWc;

    for (auto file : files)
    {
        const int arraySize = file.size() + 1;
        wchar_t* array = new wchar_t[static_cast<unsigned long long>(arraySize)];
        file.toWCharArray(array);
        qDebug() << array;
        bsa_entry_list_add(entries, array);
        bsa_add_file_from_disk(archive, bsaFolderPathWc, array);
        delete [] array;
    }

    bsa_create_archive(archive, bsaName, baSSE, entries);

    if(QFile(bsaNameQS).exists())
    {
        QLogger::QLog_Error("BsaOptimizer", tr("Cannot pack existing loose files: a BSA already exists."));
        moveFilesFromBsaFolderToRootFolder(bsaFolderPath);
    }

    //QLogger::QLog_Debug("BsaOptimizer", "BSArch Args :" + bsarchArgs.join(" ") + "\nBSA folder :" + bsaFolderPath + "\nBsaName : " + bsaName + "\nBSAsize: " + QString::number(QFile(bsaName).size()));

    /*
    if(bsarchOutput.contains("Done"))
    {
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

    bsa_save(archive);
    bsa_close(archive);

    bsa_entry_list_free(entries);
    bsa_free(archive);*/
}


bool BsaOptimizer::moveFilesFromBsaFolderToRootFolder(const QString &bsaFolderPath)
{
    QString rootFolderPath = QFileInfo(bsaFolderPath).path();

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
