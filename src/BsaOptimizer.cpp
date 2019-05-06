#include "BsaOptimizer.h"

BsaOptimizer::BsaOptimizer()
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
        QLogger::QLog_Warning("BsaOptimizer", tr("FilesToNotPack.txt not found. Animations will be packed, preventing them from being detected by FNIS and Nemesis."));
}

void BsaOptimizer::extract(QString bsaPath, const bool &deleteBackup)
{
    bsaPath = backup(bsaPath);

    QString bsaRoot(QFileInfo(bsaPath).path());
    BSArchiveAuto archive(bsaRoot);

    try {
        archive.open(bsaPath);
        archive.extractAll(bsaRoot, false);
    } catch (std::exception& e) {
        QLogger::QLog_Error("BsaOptimizer", e.what());
        QLogger::QLog_Error("BsaOptimizer", tr("An error occured during the extraction of: ") + bsaPath + "\n" + tr("Please extract it manually. The BSA was not deleted."));
        archive.close();
        return;
    }

    archive.close();

    if(deleteBackup)
        QFile::remove(bsaPath);

    QLogger::QLog_Info("BsaOptimizer", tr("BSA successfully extracted: ") + bsaPath);
}


void BsaOptimizer::create(const QString &bsaPath, QStringList &files)
{
    QDir bsaDir(QFileInfo(bsaPath).path());

    //Checking if a bsa already exists

    if(QFile(bsaPath).exists())
    {
        QLogger::QLog_Error("BsaOptimizer", tr("Cannot pack existing loose files: a BSA already exists."));
        return;
    }

    BSArchiveAuto archive(bsaDir.path());
    archive.setShareData(true);
    archive.setCompressed(true);

    //Detecting if BSA will contain sounds, since compressing BSA breaks sounds. Same for strings, Wrye Bash complains

    for (const auto& file: files)
    {
        if(file.endsWith(".wav", Qt::CaseInsensitive) || file.endsWith(".xwm", Qt::CaseInsensitive)
                || file.contains(QRegularExpression("^.+\\.[^.]*strings$")))
        {
            archive.setCompressed(false);
        }

        QString filename = QFileInfo(file).fileName();

        if(bsaDir.filePath(filename) == file)
            files.removeAll(file);
    }

    try {
        archive.addFileFromDisk(files) ;
    } catch (std::exception& e) {
        QLogger::QLog_Error("BsaOptimizer", e.what());
        QLogger::QLog_Error("BsaOptimizer", "Cancelling packing of: " + bsaPath);
        archive.close();
        return;
    }

    //Creating the archive

    try {
        archive.create(bsaPath, baSSE);
        archive.save();
        archive.close();
    } catch (std::exception e) {
        QLogger::QLog_Error("BsaOptimizer", e.what());
        QLogger::QLog_Error("BsaOptimizer", "Cancelling packing of: " + bsaPath);
        archive.close();
        return;
    }

    QLogger::QLog_Debug("BsaOptimizer", "\nBSA folder :" + bsaPath + "\nBsaName : " + bsaPath + "\nBSAsize: "
                        + QString::number(QFile(bsaPath).size()));

    //Checking if the archive is below 2.15gb, since a BSA cannot be greater

    if(QFile(bsaPath).size() < LONG_MAX)
    {
        QLogger::QLog_Note("BsaOptimizer", tr("BSA successfully compressed: ") + bsaPath);
        for (const auto& file : files)
            QFile::remove(file);
    }
    else
    {
        QLogger::QLog_Error("BsaOptimizer", tr("The BSA was not compressed: it is over 2.15gb: ") + bsaPath);
        QFile::remove(bsaPath);
    }
}

void BsaOptimizer::packAll(const QString &folderPath)
{
    QLogger::QLog_Trace("BsaOptimizer", "Entering " + QString(__FUNCTION__) + " function"
                        + "Packing all loose files into BSAs");

    //Used to name the BSA
    QString texturesBsaPath = folderPath + "/" + PluginsOperations::findPlugin(folderPath, texturesBsa) + " - Textures.bsa";;
    QString bsaPath = folderPath + "/" + PluginsOperations::findPlugin(folderPath, standardBsa) + ".bsa";;

    //Used to list files
    QStringList textures, other;
    QDirIterator it(folderPath, QDirIterator::Subdirectories);

    //Used for sizes
    QPair <qint64, qint64> size; size.first = 0; size.second = 0; //First will contain the size of textures, second will contain other assets
    const double maxTextures = 2547483647.0;
    const double maxOther = 2107483647.0;

    while (it.hasNext())
    {
        QFileInfo currentFile (it.next());
        bool doNotPack = false;

        //Checking if this file should be ignored

        for(const auto& fileToNotPack : filesToNotPack)
        {
            if(it.fileName().contains(fileToNotPack))
            {
                doNotPack = true;
                break;
            }
        }

        if(allAssets.contains(it.fileName().right(3), Qt::CaseInsensitive) && !currentFile.isDir())
        {
            bool isTexture = texturesAssets.contains(it.fileName().right(3)); //If false, it means that it's a "standard" asset

            //Using pointers to avoid duplicating the code
            QString &pBsa = isTexture ? texturesBsaPath : bsaPath;
            qint64 &pSize = isTexture ? size.first : size.second;
            QStringList &pAssets = isTexture ? textures : other;
            const double &pMaxSize = isTexture ? maxTextures : maxOther;

            if(pSize > pMaxSize){
                //Each time the maximum size is reached, a BSA is created

                pBsa = folderPath + "/" + PluginsOperations::findPlugin(folderPath, isTexture ? texturesBsa : standardBsa) + ".esp";
                if(isTexture) pBsa = pBsa.chopped(4) + " - Textures.bsa";
                create(pBsa, pAssets);

                //Resetting for next loop
                pSize = 0;
                pAssets.clear();
            }
            //adding files and sizes to list
            pAssets << it.filePath();
            pSize += currentFile.size();
        }
    }

    //Since the maximum size wasn't reached for the last archive, some files are still unpacked
    if (!textures.isEmpty())
    {
        QString texturesBsaPath = folderPath + "/" + PluginsOperations::findPlugin(folderPath, texturesBsa) + " - Textures.bsa";;
        create(texturesBsaPath, textures);
    }
    if (!other.isEmpty())
    {
        QString bsaPath = folderPath + "/" + PluginsOperations::findPlugin(folderPath, standardBsa) + ".bsa";;
        create(bsaPath, other);
    }
}


QString BsaOptimizer::backup(const QString& bsaPath) const
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
