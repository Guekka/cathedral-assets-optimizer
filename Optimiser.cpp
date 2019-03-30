#include "Optimiser.h"

using namespace QLogger;

Optimiser::Optimiser(){}


bool Optimiser::init() //Some necessary operations before running
{     
    logManager = QLoggerManager::getInstance();
    logManager->addDestination("log.html", "Optimiser", logLevel);

    saveSettings();

    modDirs.clear();
    emit progressBarReset();

    //Preparing logging

    QLog_Info("Optimiser", tr("Beginning..."));
    printSettings();

    //Disabling BSA process if Skyrim folder is choosed

    if(options.userPath == findSkyrimDirectory() + "/data" && (options.bBsaExtract || options.bBsaCreate))
    {
        QLog_Error("Optimiser", tr("You are currently in the Skyrim directory. BSA won't be processed"));
        options.bBsaExtract = false;
        options.bBsaCreate = false;
        options.bBsaPackLooseFiles = false;
        options.bBsaDeleteBackup = false;
    }

    //Checking if all the requirements are in the resources folder

    QStringList requirements;
    QFile havokFile(findSkyrimDirectory() + "/Tools/HavokBehaviorPostProcess/HavokBehaviorPostProcess.exe");

    if(havokFile.exists() && !QFile("resources/HavokBehaviorPostProcess.exe").exists())
        havokFile.copy("resources/HavokBehaviorPostProcess.exe");

    else if(!havokFile.exists() && !QFile("resources/HavokBehaviorPostProcess.exe").exists())
        QLog_Error("Optimiser", tr("Havok Tool not found. Are you sure the Creation Kit is installed ? You can also put HavokBehaviorPostProcess.exe in the resources folder"));

    requirements << "bsarch.exe" << "NifScan.exe" << "nifopt.exe" << "texconv.exe" << "texdiag.exe" << "ListHeadParts.exe";

    if(options.bAnimationsOptimization)
        requirements << "HavokBehaviorPostProcess.exe";

    for (int i = 0; i < requirements.size(); ++i)
    {
        QFile file("resources/" + requirements.at(i));
        if(!file.exists())
        {
            QLog_Error("Optimiser", requirements.at(i) + tr(" not found. Cancelling."));
            return false;
        }
    }

    //Reading custom headparts

    QFile customHeadpartsFile("resources/customHeadparts.txt");
    QString readLine;
    if(customHeadpartsFile.open(QIODevice::ReadOnly))
    {
        QTextStream ts(&customHeadpartsFile);
        while (!ts.atEnd())
        {
            readLine = QDir::cleanPath(ts.readLine());
            if(readLine.left(1) != "#")
                customHeadparts << readLine;
        }
    }
    else
        QLog_Warning("Optimiser", tr("No custom headparts file found. If you haven't created one, please ignore this message."));

    //Adding all dirs to process to modDirs

    if(options.mode == 1) //Several mods mode
    {
        QDir dir(options.userPath);
        dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
        modDirs = dir.entryList();
    }

    if(options.mode == 0) //One mod mode
        modDirs << ""; //if modDirs is empty, the loop won't be run

    modpathDir.setPath(options.userPath + "/" + modDirs.at(0)); //In case it is ran from debug UI
    QLog_Debug("Optimiser", "Modpath: " + modpathDir.path() + "\n");

    emit progressBarMaximumChanged((modDirs.size()*(options.bBsaExtract + 1 + options.bBsaCreate)));


    return true;
}


int Optimiser::mainProcess() // Process the userPath according to all user options
{
    if(!init())
    {
        emit end();
        QLog_Fatal("Optimiser", tr("The setup function did not run as expected. Exiting the process."));
        return 1;
    }

    if(options.bDryRun)
    {
        dryRun();
        emit end();
        return 2;
    }

    //This loop applies the selected optimizations to each mod specified in ModDirs

    for(int i=0; i < modDirs.size(); ++i)
    {
        modpathDir.setPath(options.userPath + "/" + modDirs.at(i));
        QDirIterator it(modpathDir, QDirIterator::Subdirectories);

        QLog_Debug("Optimiser", "ModDirs size: " + QString::number(modDirs.size()) + "\nCurrent index: " + QString::number(i));
        QLog_Info("Optimiser", tr("Current mod: ") + modpathDir.path());

        meshesList();

        if(options.bBsaExtract)
        {
            QLog_Info("Optimiser", tr("Extracting BSA..."));

            QDirIterator bsaIt(modpathDir);

            while(bsaIt.hasNext())
            {
                if(bsaIt.next().right(4) == ".bsa")
                {
                    QLog_Note("Optimiser", tr("BSA found ! Extracting...(this may take a long time, do not force close the program): ") + bsaIt.fileName());

                    try
                    {
                        bsaExtract(bsaIt.filePath());
                    }
                    catch(QString e)
                    {
                        QLog_Error("Optimiser", e);
                    }
                }
            }
            emit progressBarIncrease();
        }

        QLog_Info("Optimiser", tr("Optimizing animations, textures and meshes..."));

        QString currentFile;

        while(it.hasNext())
        {
            emit progressBarBusy();

            currentFile = it.next();

            if((options.bMeshesNecessaryOptimization || options.bMeshesMediumOptimization || options.bMeshesFullOptimization) && it.fileName().right(4).toLower() == ".nif")
                meshesOptimize(currentFile);

            if((options.bTexturesFullOptimization) && it.fileName().right(4).toLower() == ".dds")
                texturesBc7Conversion(currentFile);

            if((options.bTexturesNecessaryOptimization) && it.fileName().right(4).toLower() == ".tga")
                texturesTgaToDds(currentFile);

            if(options.bAnimationsOptimization && it.fileName().right(4).toLower() == ".hkx")
                animationsOptimize(currentFile);
        }

        emit progressBarMaximumChanged((modDirs.size()*(options.bBsaExtract + 1 + options.bBsaCreate)));
        progressBarIncrease();

        if(options.bBsaCreate)
        {
            try
            {
                if (options.bBsaPackLooseFiles)
                    splitAssets();

                QDirIterator bsaIt(modpathDir);
                while(bsaIt.hasNext())
                {
                    bsaIt.next();
                    if(bsaIt.fileName().right(13) == "bsa.extracted")
                    {
                        QLog_Trace("Optimiser", "bsa folder found: " + bsaIt.fileName());
                        bsaCreate(bsaIt.filePath());
                    }
                }
            }
            catch (QString e)
            {
                QLog_Error("Optimiser", e);
                QLog_Error("Optimiser", tr("BSA packing canceled."));
            }
            emit progressBarIncrease();
        }
    }

    //Deleting empty dirs

    system(qPrintable("cd /d \"" + options.userPath + R"(" && for /f "delims=" %d in ('dir /s /b /ad ^| sort /r') do rd "%d" >nul 2>nul)"));

    emit end();

    QLog_Info("Optimiser", tr("Completed."));

    return 0;
}


void Optimiser::dryRun() // Perform a dry run : list files without actually modifying them
{
    init();

    QLog_Info("Optimiser", tr("Beginning the dry run..."));

    for(int i=0; i < modDirs.size(); ++i)
    {
        modpathDir.setPath(QDir::cleanPath(options.userPath + "/" + modDirs.at(i)));
        QDirIterator it(modpathDir, QDirIterator::Subdirectories);

        meshesList();

        QLog_Info("Optimiser", tr("Current mod: ") + modpathDir.path());

        if(options.bBsaExtract)
        {
            QLog_Info("Optimiser", tr("Extracting BSA..."));

            QDirIterator bsaIt(modpathDir);

            while(bsaIt.hasNext())
            {
                if(bsaIt.next().right(4) == ".bsa")
                {
                    QLog_Note("Optimiser", tr("BSA found ! Extracting...(this may take a long time, do not force close the program): ") + bsaIt.fileName());
                    try
                    {
                        bsaExtract(bsaIt.filePath());
                    }
                    catch(QString e)
                    {
                        QLog_Error("Optimiser", e);
                    }
                }
            }
            emit progressBarIncrease();
        }

        while(it.hasNext())
        {
            it.next();


            if(it.fileName().contains(".nif", Qt::CaseInsensitive))
            {
                if(options.bMeshesNecessaryOptimization && headparts.contains(it.filePath(), Qt::CaseInsensitive))
                    QLog_Info("Optimiser", it.filePath() + tr(" would be optimized by Headparts meshes option"));

                else if(options.bMeshesMediumOptimization && otherMeshes.contains(it.filePath()))
                    QLog_Info("Optimiser", it.filePath() + tr(" would be optimized lightly by the Other Meshes option"));

                else if(options.bMeshesNecessaryOptimization && crashingMeshes.contains(it.filePath(), Qt::CaseInsensitive))
                    QLog_Info("Optimiser", it.filePath() + tr(" would be optimized in full by the Hard Crashing Meshes option."));

                else if(options.bMeshesFullOptimization)
                    QLog_Info("Optimiser", it.filePath() + tr(" would be optimized lightly by the Other Meshes option"));
            }
            if((options.bTexturesFullOptimization) && it.fileName().contains(".dds", Qt::CaseInsensitive))
            {
                QProcess texDiag;
                QStringList texdiagArg;

                texdiagArg << "info" << it.filePath();

                texDiag.start("resources/texdiag.exe", texdiagArg);
                texDiag.waitForFinished(-1);

                if(texDiag.readAllStandardOutput().contains("compressed = no"))
                    QLog_Info("Optimiser", it.filePath() + tr(" would be optimized using BC7 compression."));
            }

            if((options.bTexturesNecessaryOptimization) && it.fileName().contains(".tga", Qt::CaseInsensitive))
                QLog_Info("Optimiser", it.filePath() + tr(" would be converted to DDS"));
        }

        emit progressBarIncrease();


    }
    QLog_Info("Optimiser", tr("Completed."));
}


void Optimiser::bsaExtract(const QString& bsaPath) //Extracts all BSA in modPath
{
    QProcess bsarch;
    QStringList bsarchArgs;
    QStringList files(modpathDir.entryList());

    QString bsaFolder = modpathDir.filePath(bsaPath + ".extracted");

    modpathDir.mkdir(bsaFolder);

    if(options.bBsaDeleteBackup)
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

    bsarch.start("resources/bsarch.exe", bsarchArgs);
    bsarch.waitForFinished(-1);

    QLog_Debug("Optimiser", "BSArch Args :" + bsarchArgs.join(" ") + "\nBSA folder :" + bsaFolder + "\nBSA Name :" + bsaPath);

    if(!bsarch.readAllStandardOutput().contains("Done"))
        throw tr("An error occured during the extraction. Please extract it manually. The BSA was not deleted.");
    else
    {
        if(!options.bBsaCreate)
        {
            try
            {
                moveFiles(bsaFolder, modpathDir.path(), false);
            }
            catch (QString e)
            {
                QLog_Error("Optimiser", e);
                throw tr("An error occured during the extraction. Please extract it manually. The BSA was not deleted.");
            }
        }
        if(options.bBsaDeleteBackup)
            QFile::remove(bsaPath);
    }
}


void Optimiser::bsaCreate(const QString& bsaFolderPath) //Once all the optimizations are done, create a new BSA
{
    QStringList dirs(modpathDir.entryList(QDir::Dirs));

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
        bsarch.start("resources/bsarch.exe", bsarchArgs);
        bsarch.waitForFinished(-1);
    }
    else
    {
        QLog_Error("Optimiser", tr("Cannot pack existing loose files: a BSA already exists."));
        try
        {
            moveFiles(bsaFolderPath, modpathDir.path(), false);
        }
        catch (QString e)
        {
            QLog_Error("Optimiser", e);
        }
    }

    QLog_Debug("Optimiser", "BSArch Args :" + bsarchArgs.join(" ") + "\nBSA folder :" + bsaFolderPath + "\nBsaName : " + bsaName + "\nBSAsize: " + QString::number(QFile(bsaName).size()));

    if(bsarch.readAllStandardOutput().contains("Done"))
    {
        if(QFile(bsaName).size() < 2147483648)
        {
            QLog_Note("Optimiser", tr("BSA successfully compressed: ") + bsaName);
            bsaDir.setPath(bsaFolderPath);
            bsaDir.removeRecursively();
        }
        else if(QFile(bsaName).size() < 2400000000)
        {
            QLog_Warning("Optimiser", tr("The BSA is nearly over its maximum size. It still should work."));
            bsaDir.setPath(bsaFolderPath);
            bsaDir.removeRecursively();
        }
        else
        {
            QLog_Error("Optimiser", tr("The BSA was not compressed: it is over 2.2gb: ") + bsaName);
            QFile::remove(bsaName);
            if(QFile(bsaName.chopped(3) + "esp").size() == QFile("resources/BlankSSEPlugin.esp").size())
                QFile::remove(modpathDir.filePath(bsaName.chopped(3) + "esp"));
            try
            {
                moveFiles(bsaFolderPath, modpathDir.path(), false);
            }
            catch (QString e)
            {
                QLog_Error("Optimiser", e);
            }
        }
    }
}


void Optimiser::texturesBc7Conversion(const QString &filePath) //Compress uncompressed textures to BC7
{
    QProcess texDiag;
    QStringList texdiagArg;

    QProcess texconv;
    QStringList texconvArg;

    texdiagArg << "info" << filePath;

    texDiag.start("resources/texdiag.exe", texdiagArg);
    texDiag.waitForFinished(-1);

    QString texDiagOutput = texDiag.readAllStandardOutput();

    if(texDiagOutput.contains("compressed = no"))
    {
        QString width = texDiagOutput.mid(texDiagOutput.indexOf("width = ")+8, 4);
        QString height = texDiagOutput.mid(texDiagOutput.indexOf("height = ")+9, 4);
        int textureSize = width.trimmed().toInt() * height.trimmed().toInt();

        if(textureSize > 16)
        {
            texconvArg.clear();
            texconvArg << "-nologo" << "-y" << "-m" << "0" << "-pow2" << "-if" << "FANT" << "-f" << "BC7_UNORM" << "-bcmax" << filePath;

            QLog_Note("Optimiser", tr("Compressing uncompressed texture: ") + filePath.mid(filePath.lastIndexOf("/")+1));
            texconv.start("resources/texconv.exe", texconvArg);
            texconv.waitForFinished(-1);
        }
    }
}


void Optimiser::texturesTgaToDds(const QString &filePath) //Convert TGA textures to DDS
{
    QLog_Note("Optimiser", tr("Converting TGA files..."));

    QProcess texconv;
    QStringList texconvArg;

    QLog_Note("Optimiser", tr("TGA file found: ") + filePath.mid(filePath.lastIndexOf("/")) + tr("Compressing..."));


    texconvArg.clear();
    texconvArg << "-nologo" << "-m" << "0" << "-pow2" << "-if" << "FANT" << "-f" << "R8G8B8A8_UNORM" << filePath;
    texconv.start("resources/texconv.exe", texconvArg);
    texconv.waitForFinished(-1);

    QFile tga(filePath);
    tga.remove();

}


void Optimiser::meshesList() //Run NifScan on modPath. Detected meshes will be stored to a list, accorded to their types.
{
    QLog_Note("Optimiser", tr("Listing meshes..."));

    crashingMeshes.clear();
    otherMeshes.clear();
    headparts.clear();

    QString readLine;
    QString currentFile;

    QFile nifScan_file("resources/NifScan.exe");

    QProcess nifScan;
    QProcess listHeadparts;

    QStringList listHeadpartsArgs;
    QStringList nifscanArgs;

    //Running Nifscan and ListHeadparts to fill lists

    nifScan.setReadChannel(QProcess::StandardOutput);
    nifscanArgs << modpathDir.path() << "-fixdds";

    nifScan.start("resources/NifScan.exe", nifscanArgs);

    if(!nifScan.waitForFinished(180000))
        QLog_Error("Optimiser", tr("Nifscan has not finished withing 3 minutes. Skipping mesh optimization for this mod."));

    while(nifScan.canReadLine())
    {
        readLine=QString::fromLocal8Bit(nifScan.readLine());

        if(readLine.contains("meshes\\", Qt::CaseInsensitive))
        {
            currentFile = QDir::cleanPath(readLine.simplified());
            if(currentFile.contains("facegendata"))
                headparts << modpathDir.filePath(currentFile);
            else
                otherMeshes << modpathDir.filePath(currentFile);
        }

        else if(readLine.contains("unsupported", Qt::CaseInsensitive) || readLine.contains("not supported", Qt::CaseInsensitive))
        {
            crashingMeshes << modpathDir.filePath(currentFile);;
            otherMeshes.removeAll(modpathDir.filePath(currentFile));
        }
    }


    listHeadpartsArgs << modpathDir.path();
    listHeadparts.start("resources/ListHeadParts.exe", listHeadpartsArgs);
    listHeadparts.waitForFinished(-1);

    while(listHeadparts.canReadLine())
    {
        readLine=QString::fromLocal8Bit(listHeadparts.readLine());
        headparts << modpathDir.filePath(readLine.simplified());
    }

    //Adding custom headparts to detected headparts

    for(int i = 0; i < customHeadparts.size(); ++i)
    {
        headparts << QDir::cleanPath(modpathDir.filePath(customHeadparts.at(i)));
    }

    //Removing hard crashing meshes from other meshes list

    QStringListIterator it(crashingMeshes);

    while (it.hasNext())
    {
        otherMeshes.removeAll(it.next());
    }

    QStringListIterator it2(headparts);
    QString temp;

    while(it2.hasNext())
    {
        temp = it2.next();
        otherMeshes.removeAll(temp);
        crashingMeshes.removeAll(temp);
    }

    //Cleaning the lists

    headparts.removeDuplicates();
    otherMeshes.removeDuplicates();
    crashingMeshes.removeDuplicates();

    headparts.removeAll("");
    otherMeshes.removeAll("");
    crashingMeshes.removeAll("");

    QLog_Trace("Optimiser", "Headparts list:\n\n" + headparts.join("\n") + "\n\nCrashing meshes list:\n\n" + crashingMeshes.join("\n") + "\n\nOther meshes list:\n\n" + otherMeshes.join("\n"));
}


void Optimiser::meshesOptimize(const QString &filePath) // Optimize the selected mesh
{
    QProcess nifOpt;
    QStringList nifOptArgs;

    if(options.bMeshesNecessaryOptimization && headparts.contains(filePath, Qt::CaseInsensitive) && options.bMeshesHeadparts)
    {
        crashingMeshes.removeAll(filePath);
        nifOptArgs << filePath << "-head" << "1" << "-bsTriShape" << "1";
        QLog_Note("Optimiser", tr("Running NifOpt...")  + tr("Processing: ") + filePath + tr(" as an headpart due to crashing meshes option"));
    }

    else if(options.bMeshesNecessaryOptimization && crashingMeshes.contains(filePath, Qt::CaseInsensitive))
    {
        nifOptArgs << filePath << "-head" << "0" << "-bsTriShape" << "1";
        QLog_Note("Optimiser", tr("Running NifOpt...")  + tr("Processing: ") + filePath + tr(" due to crashing meshes option"));
    }

    else if(options.bMeshesFullOptimization && otherMeshes.contains(filePath, Qt::CaseInsensitive))
    {
        nifOptArgs << filePath << "-head" << "0" << "-bsTriShape" << "1";
        QLog_Note("Optimiser", tr("Running NifOpt...") + tr("Processing: ") + filePath + tr(" due to all meshes option"));
    }

    else if(options.bMeshesMediumOptimization && otherMeshes.contains(filePath, Qt::CaseInsensitive))
    {
        nifOptArgs << filePath << "-head" << "0" << "-bsTriShape" << "0";
        QLog_Note("Optimiser", tr("Running NifOpt...")  + tr("Processing: ") + filePath + tr(" due to other meshes option"));
    }

    else if(options.bMeshesFullOptimization)
    {
        nifOptArgs << filePath << "-head" << "0" << "-bsTriShape" << "1";
        QLog_Note("Optimiser", tr("Running NifOpt...")  + tr("Processing: ") + filePath + tr(" due to all meshes option"));
    }

    nifOpt.start("resources/nifopt.exe", nifOptArgs);
    nifOpt.waitForFinished(-1);
}


void Optimiser::meshesTexturesCaseFix(const QString &filePath) //Unused. Work in progress. Same func as NIF Texcase Fixer
{
    QFile file(filePath);
    QString binaryData;
    QString texturePath;
    QStringList storedTextures;
    QVector <QStringRef> matches;

    QDirIterator textures(modpathDir, QDirIterator::Subdirectories);

    while (textures.hasNext())
    {
        if(textures.next().right(3).toLower() == "dds")
            storedTextures << modpathDir.relativeFilePath(textures.filePath());
    }

    file.open(QFile::ReadWrite);
    binaryData = QTextCodec::codecForMib(106)->toUnicode(file.read(999999));

    qDebug() << filePath;

    if(binaryData.contains(".dds"))
    {
        matches = binaryData.splitRef(QRegularExpression(R"(?:[a-zA-Z]:(?:.*?))?textures(?:.*?)dds)"));
        for (const auto& match : matches)
        {
            for (const auto& tex : storedTextures)
            {
                if(match == tex)
                    break;

                else if(match.toString().toLower() == tex.toLower())
                    binaryData.replace(match.toString().toUtf8(), tex.toUtf8());

                else if(match.endsWith(tex, Qt::CaseInsensitive))
                {
                    binaryData.replace(match.toString().toUtf8(), tex.toUtf8());
                }
            }
        }
    }

    file.close();
}


void Optimiser::animationsOptimize(const QString &filePath) //Run Bethesda Havok Tool to port the selected animation
{
    QProcess havokProcess;
    QStringList havokArgs;

    havokArgs.clear();
    havokArgs << "--platformamd64" << filePath << filePath;
    havokProcess.start("resources/HavokBehaviorPostProcess.exe", havokArgs);

    havokProcess.waitForFinished(-1);

    if(havokProcess.readAllStandardOutput().isEmpty())
        QLog_Note("Optimiser", tr("Animation successfully ported: ") + filePath);
}


QString Optimiser::findSkyrimDirectory() //Find Skyrim directory using the registry key
{
    QSettings SkyrimReg(R"(HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Bethesda Softworks\Skyrim Special Edition)", QSettings::NativeFormat);
    QDir SkyrimDir = QDir::cleanPath(SkyrimReg.value("Installed Path").toString());
    return SkyrimDir.path();
}


QString Optimiser::getPlugin() //Find esp/esl/esm name using an iterator and regex. Also creates a plugin if there isn't one.
{
    QDirIterator it(modpathDir);
    QString espName;

    while (it.hasNext())
    {
        it.next();

        if(it.fileName().contains(QRegularExpression("\\.es[plm]")))
        {
            espName=it.fileName();
            QLog_Note("Optimiser", tr("Esp found: ") + espName);
            return espName;
        }
    }
    espName = modpathDir.dirName();
    return espName;
}


void Optimiser::splitAssets() //Split assets between several folders
{
    QLog_Trace("Optimiser", "Entering splitAssets function");

    QStringList assets;
    QString relativeFilename;
    QString espName = getPlugin().chopped(4);

    QStringList bsaList;
    QStringList texturesBsaList;
    QStringList dirs(modpathDir.entryList(QDir::Dirs));

    QDirIterator it(modpathDir, QDirIterator::Subdirectories);

    QFile oldAsset;
    QFile newAsset;

    assets << "nif" << "seq" << "pex" << "psc" << "lod" << "fuz" << "waw" << "xwm" << "swf" << "hkx" << "wav" << "tri" << "btr" << "bto" << "btt" << "lip";

    QLog_Trace("Optimiser", "Listing all BSA folders and moving files to modpath root directory");

    for (int i = 0; i < dirs.size(); ++i)
    {
        if(dirs.at(i).right(13) == "bsa.extracted" && dirs.at(i).contains("- Textures", Qt::CaseInsensitive))
        {
            texturesBsaList << modpathDir.filePath(dirs.at(i));
            moveFiles(modpathDir.filePath(dirs.at(i)), modpathDir.path(), false);
        }
        else if(dirs.at(i).right(13) == "bsa.extracted")
        {
            bsaList << modpathDir.filePath(dirs.at(i));
            moveFiles(modpathDir.filePath(dirs.at(i)), modpathDir.path(), false);
        }
    }

    if(options.bBsaSplitAssets)
    {
        QLog_Trace("Optimiser", "Creating enough folders to contain all the files");

        QPair<qint64, qint64> size = assetsSize(modpathDir.path());

        int i = 0;
        while(texturesBsaList.size() < qCeil(size.first/2900000000.0))
        {
            ++i;
            modpathDir.mkdir(espName + QString::number(i) + " - Textures.bsa.extracted");
            texturesBsaList << espName + QString::number(i) + " - Textures.bsa.extracted";
            texturesBsaList.removeDuplicates();
        }
        i = 0;
        while(bsaList.size() < qCeil(size.second/2076980377.0))
        {
            ++i;
            modpathDir.mkdir(espName + QString::number(i) + ".bsa.extracted");
            bsaList << espName + QString::number(i) + ".bsa.extracted" ;
            bsaList.removeDuplicates();
        }

        QLog_Trace("Optimiser", "Total: " + QString::number(bsaList.size()) + " bsa folders and " + QString::number(texturesBsaList.size()) + " textures bsa folders");
    }
    else
    {
        if(texturesBsaList.isEmpty())
            texturesBsaList << modpathDir.dirName() + " - Textures.bsa.extracted";
        if(bsaList.isEmpty())
            bsaList << modpathDir.dirName() + ".bsa.extracted";
    }

    QLog_Trace("Optimiser", "Splitting files between bsa folders");

    int i = 0;
    int j = 0;

    while(it.hasNext())
    {
        it.next();

        if(assets.contains(it.fileName().right(3), Qt::CaseInsensitive) || it.fileName().right(3).toLower() == "dds" || it.fileName().right(3).toLower() == "png")
        {
            if(assets.contains(it.fileName().right(3), Qt::CaseInsensitive))
            {
                ++i;
                if(i >= bsaList.size() || i < 0)
                    i = 0;
                newAsset.setFileName(modpathDir.filePath(bsaList.at(i) + "/" + relativeFilename));
            }

            else if(it.fileName().right(3).toLower() == "dds" || it.fileName().right(3).toLower() == "png")
            {
                ++j;
                if(j >= texturesBsaList.size() || j < 0)
                    j = 0;
                newAsset.setFileName(modpathDir.filePath(texturesBsaList.at(j) + "/" + relativeFilename));
            }

            oldAsset.setFileName(it.filePath());
            relativeFilename = modpathDir.relativeFilePath(it.filePath());

            //removing the duplicate assets and checking for path size

            if(newAsset.fileName().size() >= 260) //Max path size for Windows
                throw tr("The filepath is more than 260 characters long. Please reduce it.");

            else if(oldAsset.size() == newAsset.size())
                QFile::remove(newAsset.fileName());

            modpathDir.mkpath(newAsset.fileName().left(newAsset.fileName().lastIndexOf("/")));
            modpathDir.rename(oldAsset.fileName(), newAsset.fileName());
        }
    }

    QLog_Trace("Optimiser", "Creating dummy plugins to load BSAs");

    for (int i = 0; i < bsaList.size(); ++i)
    {
        if(!QFile(bsaList.at(i).chopped(13) + "esp").exists())
        {
            QFile::copy("resources/BlankSSEPlugin.esp", modpathDir.path() + "/" + bsaList.at(i).chopped(3) + "esp");
            QLog_Trace("Optimiser", "Created standard bsa plugin:" + bsaList.at(i).chopped(3) + "esp");
        }
    }
    for (int i = 0; i < texturesBsaList.size(); ++i)
    {
        if(!QFile(texturesBsaList.at(i).chopped(25) + ".esp").exists())
        {
            QFile::copy("resources/BlankSSEPlugin.esp", modpathDir.path() + "/" + texturesBsaList.at(i).chopped(25) + ".esp");
            QLog_Trace("Optimiser", "Created textures bsa plugin:" + texturesBsaList.at(i).chopped(25) + ".esp");
        }
    }

    system(qPrintable("cd /d \"" + options.userPath + R"(" && for /f "delims=" %d in ('dir /s /b /ad ^| sort /r') do rd "%d" >nul 2>nul)"));

    QLog_Trace("Optimiser", "Exiting splitAssets function");
}


void Optimiser::moveFiles(QString source, QString destination, bool overwriteExisting) //FIXME the last file is not moved
{
    QString relativeFilename;

    QDir sourceDir(source);
    QDirIterator it(source, QDirIterator::Subdirectories);

    QFile oldFile;
    QFile newFile;

    source = QDir::cleanPath(source) + "/";
    destination = QDir::cleanPath(destination) + "/";
    QLog_Trace("Optimiser", "Entering moveFiles function");
    QLog_Debug("Optimiser", "dest folder: " + destination + "\nsource folder: " + source);

    sourceDir.mkdir(destination);

    while (it.hasNext())
    {
        it.next();
        if(it.path() != destination && it.fileName().right(1) != ".")
        {
            relativeFilename = QDir::cleanPath(sourceDir.relativeFilePath(it.filePath()));
            oldFile.setFileName(it.filePath());
            newFile.setFileName(destination + relativeFilename);

            //removing the duplicate files from new folder (if overwriteExisting) or from old folder (if !overwriteExisting)

            if(newFile.fileName().size() >= 260)
                throw tr("An error occurred while moving files. Try reducing path size (260 characters is the maximum)");

            else if(oldFile.size() == newFile.size() && overwriteExisting)
                QFile::remove(newFile.fileName());
            else if(oldFile.size() == newFile.size() && !overwriteExisting)
                QFile::remove(oldFile.fileName());

            sourceDir.mkpath(newFile.fileName().left(newFile.fileName().lastIndexOf("/")));
            QFile::rename(oldFile.fileName(), newFile.fileName());
        }
    }
    QLog_Trace("Optimiser", "Exiting moveFiles function");
}


QPair <qint64, qint64> Optimiser::assetsSize(const QString& path) // Return textures size and other assets size in a directory
{
    QPair <qint64, qint64> size;
    //First will be textures, second will be other assets

    QDirIterator it(path, QDirIterator::Subdirectories);
    QStringList assets;
    QFile file;

    assets << "nif" << "seq" << "pex" << "psc" << "lod" << "fuz" << "waw" << "xwm" << "swf" << "hkx" << "wav" << "tri" << "btr" << "bto" << "btt" << "lip";

    while (it.hasNext())
    {
        file.setFileName(it.next());

        if(file.fileName().right(3).toLower() == "dds" || file.fileName().right(3).toLower() == "png")
            size.first += file.size();
        else if(assets.contains(file.fileName().right(3), Qt::CaseInsensitive))
            size.second += file.size();
    }

    return size;

}



void Optimiser::saveSettings() //Saves settings to an ini file
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");


    settings.setValue("options.mode", options.mode);
    settings.setValue("DryRun", options.bDryRun);
    settings.setValue("SelectedPath", options.userPath);

    settings.setValue("logLevel", logLevelToInt(logLevel));

    settings.setValue("bBsaExtract", options.bBsaExtract);
    settings.setValue("bBsaCreate", options.bBsaCreate);
    settings.setValue("bBsaPackLooseFiles", options.bBsaPackLooseFiles);
    settings.setValue("bBsaDeleteBackup", options.bBsaDeleteBackup);
    settings.setValue("bBsaSplitAssets", options.bBsaSplitAssets);

    settings.setValue("bMeshesNecessaryOptimization", options.bMeshesNecessaryOptimization);
    settings.setValue("bMeshesMediumOptimization", options.bMeshesMediumOptimization);
    settings.setValue("bMeshesFullOptimization", options.bMeshesFullOptimization);

    settings.setValue("bTexturesNecessaryOptimization", options.bTexturesNecessaryOptimization);
    settings.setValue("bTexturesFullOptimization", options.bTexturesFullOptimization);

    settings.setValue("bAnimationsOptimization", options.bAnimationsOptimization);
}


void Optimiser::loadSettings() //Loads settings from the ini file
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");

    options.mode = settings.value("options.mode").toInt();
    options.bDryRun = settings.value("DryRun").toBool();
    options.userPath = settings.value("SelectedPath").toString();

    logLevel = intToLogLevel(settings.value("logLevel").toInt());

    options.bBsaExtract = settings.value("bBsaExtract").toBool();
    options.bBsaCreate = settings.value("bBsaCreate").toBool();
    options.bBsaPackLooseFiles = settings.value("bBsaPackLooseFiles").toBool();
    options.bBsaDeleteBackup = settings.value("bBsaDeleteBackup").toBool();
    options.bBsaSplitAssets = settings.value("bBsaSplitAssets").toBool();

    options.bMeshesNecessaryOptimization = settings.value("bMeshesNecessaryOptimization").toBool();
    options.bMeshesMediumOptimization = settings.value("bMeshesMediumOptimization").toBool();
    options.bMeshesFullOptimization = settings.value("bMeshesFullOptimization").toBool();

    options.bTexturesNecessaryOptimization = settings.value("bTexturesNecessaryOptimization").toBool();
    options.bTexturesFullOptimization = settings.value("bTexturesFullOptimization").toBool();

    options.bAnimationsOptimization = settings.value("bAnimationsOptimization").toBool();
}


void Optimiser::printSettings() //Will print settings into debug log
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");

    QLog_Debug("Optimiser", "mode: " + QString::number(options.mode));
    QLog_Debug("Optimiser", "DryRun: " + QString::number(options.bDryRun));
    QLog_Debug("Optimiser", "userPath: "+ options.userPath);

    QLog_Debug("Optimiser", "LogLevel: " + QString::number(logLevelToInt(logLevel)));

    QLog_Debug("Optimiser", "bBsaExtract: " + QString::number(options.bBsaExtract));
    QLog_Debug("Optimiser", "bBsaCreate: " + QString::number(options.bBsaCreate));
    QLog_Debug("Optimiser", "bBsaPackLooseFiles: " + QString::number(options.bBsaPackLooseFiles));
    QLog_Debug("Optimiser", "bBsaDeleteBackup: " + QString::number(options.bBsaDeleteBackup));
    QLog_Debug("Optimiser", "bBsaSplitAssets" + QString::number(options.bBsaSplitAssets));

    QLog_Debug("Optimiser", "bMeshesNecessaryOptimization: " + QString::number(options.bMeshesNecessaryOptimization));
    QLog_Debug("Optimiser", "bMeshesMediumOptimization: " + QString::number(options.bMeshesMediumOptimization));
    QLog_Debug("Optimiser", "bMeshesFullOptimization: " + QString::number(options.bMeshesFullOptimization));

    QLog_Debug("Optimiser", "bTexturesNecessaryOptimization: " + QString::number( options.bTexturesNecessaryOptimization));
    QLog_Debug("Optimiser", "bTexturesFullOptimization: " + QString::number( options.bTexturesFullOptimization));

    QLog_Debug("Optimiser", "bAnimationsOptimization: " + QString::number(options.bAnimationsOptimization));
}

void Optimiser::setLogLevel(const QLogger::LogLevel &value)
{
    logLevel = value;
}
