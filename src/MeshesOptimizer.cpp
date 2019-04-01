#include "MeshesOptimizer.h"

MeshesOptimizer::MeshesOptimizer()
{
    //Reading custom headparts file to add them to the list.
    //Done in the constructor since the file won't change at runtime.

    QFile customHeadpartsFile(QCoreApplication::applicationDirPath() + "/resources/customHeadparts.txt");
    if(customHeadpartsFile.open(QIODevice::ReadOnly))
    {
        QTextStream ts(&customHeadpartsFile);
        while (!ts.atEnd())
        {
            QString readLine = QDir::cleanPath(ts.readLine());
            if(readLine.left(1) != "#")
                customHeadparts << readLine;
        }
    }
    else
        QLogger::QLog_Warning("Optimiser", tr("No custom headparts file found. If you haven't created one, please ignore this message."));
}


void MeshesOptimizer::meshesList(const QString& folderPath) //Run NifScan on modPath. Detected meshes will be stored to a list, accorded to their types.
{
    QLogger::QLog_Note("MeshesOptimizer", tr("Listing meshes..."));

    crashingMeshes.clear();
    otherMeshes.clear();
    headparts.clear();

    QDir directory(folderPath);

    //Running Nifscan and ListHeadparts to fill lists

    QProcess nifScan;
    nifScan.start(QCoreApplication::applicationDirPath() + "/resources/NifScan.exe", QStringList() << directory.path() << "fixdds");

    if(!nifScan.waitForFinished(180000))
        QLogger::QLog_Error("MeshesOptimizer", tr("Nifscan has not finished within 3 minutes. Skipping mesh optimization for this mod."));

    while(nifScan.canReadLine())
    {
        QString readLine = QString::fromLocal8Bit(nifScan.readLine());
        QString currentFile;

        if(readLine.contains("meshes\\", Qt::CaseInsensitive) && !readLine.contains("Error", Qt::CaseInsensitive))
        {
            currentFile = QDir::cleanPath(readLine.simplified());
            if(currentFile.contains("facegendata"))
                headparts << directory.filePath(currentFile);
            else
                otherMeshes << directory.filePath(currentFile);
        }

        else if(readLine.contains("unsupported", Qt::CaseInsensitive) || readLine.contains("not supported", Qt::CaseInsensitive))
        {
            crashingMeshes << directory.filePath(currentFile);;
            otherMeshes.removeAll(directory.filePath(currentFile));
        }
    }

    listHeadparts(directory);
    cleanMeshesLists();

    //QLogger::QLog_Trace("MeshesOptimizer", "Headparts list:\n\n" + headparts.join("\n") + "\n\nCrashing meshes list:\n\n"
    // + crashingMeshes.join("\n") + "\n\nOther meshes list:\n\n" + otherMeshes.join("\n"));
}


void MeshesOptimizer::cleanMeshesLists()
{
    //Removing hard crashing meshes from other meshes list

    QStringListIterator it(crashingMeshes);

    while (it.hasNext())
    {
        otherMeshes.removeAll(it.next());
    }

    QStringListIterator it2(headparts);

    while(it2.hasNext())
    {
        QString temp = it2.next();
        otherMeshes.removeAll(temp);
        crashingMeshes.removeAll(temp);
    }

    //Removing duplicates

    headparts.removeDuplicates();
    otherMeshes.removeDuplicates();
    crashingMeshes.removeDuplicates();
}


void MeshesOptimizer::listHeadparts(const QDir& directory)
{
    QProcess listHeadparts;
    listHeadparts.start(QCoreApplication::applicationDirPath() + "/resources/ListHeadParts.exe", QStringList() << directory.path());

    if(!listHeadparts.waitForFinished(180000))
            QLogger::QLog_Error("MeshesOptimizer", tr("ListHeadparts has not finished within 3 minutes. Skipping headparts optimization for this mod."));

    while(listHeadparts.canReadLine())
    {
        QString readLine = QString::fromLocal8Bit(listHeadparts.readLine()).simplified();
        headparts << directory.filePath(readLine);
    }

    //Adding custom headparts to detected headparts

    for(int i = 0; i < customHeadparts.size(); ++i)
    {
        headparts << QDir::cleanPath(directory.filePath(customHeadparts.at(i)));
    }
}


void MeshesOptimizer::meshesOptimize(const QString &filePath) // Optimize the selected mesh
{
    QProcess nifOpt;
    QStringList nifOptArgs;

    if(bMeshesNecessaryOptimization && headparts.contains(filePath, Qt::CaseInsensitive) && bMeshesHeadparts)
    {
        crashingMeshes.removeAll(filePath);
        nifOptArgs << filePath << "-head" << "1" << "-bsTriShape" << "1";
        QLogger::QLog_Note("MeshesOptimizer", tr("Running NifOpt...")  + tr("Processing: ") + filePath + tr(" as an headpart due to crashing meshes option"));
    }

    else if(bMeshesNecessaryOptimization && crashingMeshes.contains(filePath, Qt::CaseInsensitive))
    {
        nifOptArgs << filePath << "-head" << "0" << "-bsTriShape" << "1";
        QLogger::QLog_Note("MeshesOptimizer", tr("Running NifOpt...")  + tr("Processing: ") + filePath + tr(" due to crashing meshes option"));
    }

    else if(bMeshesFullOptimization && otherMeshes.contains(filePath, Qt::CaseInsensitive))
    {
        nifOptArgs << filePath << "-head" << "0" << "-bsTriShape" << "1";
        QLogger::QLog_Note("MeshesOptimizer", tr("Running NifOpt...") + tr("Processing: ") + filePath + tr(" due to all meshes option"));
    }

    else if(bMeshesMediumOptimization && otherMeshes.contains(filePath, Qt::CaseInsensitive))
    {
        nifOptArgs << filePath << "-head" << "0" << "-bsTriShape" << "0";
        QLogger::QLog_Note("MeshesOptimizer", tr("Running NifOpt...")  + tr("Processing: ") + filePath + tr(" due to other meshes option"));
    }

    else if(bMeshesFullOptimization)
    {
        nifOptArgs << filePath << "-head" << "0" << "-bsTriShape" << "1";
        QLogger::QLog_Note("MeshesOptimizer", tr("Running NifOpt...")  + tr("Processing: ") + filePath + tr(" due to all meshes option"));
    }

    nifOpt.start(QCoreApplication::applicationDirPath() + "/resources/nifopt.exe", nifOptArgs);
    nifOpt.waitForFinished(-1);
}

