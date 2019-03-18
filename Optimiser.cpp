#include "Optimiser.hpp"

const QString errorColor("<font color=Red>");
const QString stepsColor("<font color=Green>");
const QString noteColor("<font color=Grey>");
const QString currentModColor("<font color=Orange>");
const QString endColor("</font>\n");

Optimiser::Optimiser(QTextEdit* textedit, QTextEdit* debuglog, QProgressBar* bar) : log(textedit), debugLog(debuglog), progressBar(bar){}

Optimiser::~Optimiser()
{
    delete trash;
}


bool Optimiser::setup() //Some necessary operations before running
{
    saveSettings();
    printSettings();

    modDirs.clear();
    progressBar->reset();
    log->clear();
    debugLog->clear();

    //Disabling BSA process if Skyrim folder is choosed

    if(modPath == findSkyrimDirectory() + "/data" && (options.bBsaExtract || options.bBsaCreate))
    {
        log->append(errorColor + "You are currently in the Skyrim Dir. BSA won't be processed");
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
        log->append(errorColor + tr("Havok Tool not found. Are you sure the Creation Kit is installed ? You can also put HavokBehaviorPostProcess.exe in the resources folder") + endColor);

    requirements << "bsarch.exe" << "NifScan.exe" << "nifopt.exe" << "texconv.exe" << "texdiag.exe" << "ListHeadParts.exe";

    if(options.bAnimationsOptimization)
        requirements << "HavokBehaviorPostProcess.exe";

    for (int i = 0; i < requirements.size(); ++i)
    {
        QFile file("resources/" + requirements.at(i));
        if(!file.exists())
        {
            log->append(errorColor + requirements.at(i) + tr(" not found. Cancelling.") + endColor);
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
        log->append(noteColor + "No custom headparts file found. If you haven't created one, please ignore this message." + endColor);

    //Adding all dirs to process to modDirs

    if(options.mode == 1) //Several mods mode
    {
        QDir dir(options.userPath);
        dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
        modDirs = dir.entryList();
    }

    if(options.mode == 0) //One mod mode
        modDirs << ""; //if modDirs is empty, the loop won't be run

    modPath = QDir::cleanPath(options.userPath + "/" + modDirs.at(0)); //In case it is ran from debug UI
    debugLog->append(stepsColor + "[SETUP FUNC]" + endColor + "Modpath: " + modPath + "\n" + stepsColor + "[/SETUP FUNC]" + endColor);

    progressBar->setMaximum(modDirs.size()*(options.bBsaExtract + 1 + options.bBsaCreate));

    QCoreApplication::processEvents();
    return true;
}


int Optimiser::mainProcess() // Process the userPath according to all user options
{
    if(!setup())
        return 1;

    if(options.bDryRun)
    {
        dryRun();
        return 2;
    }

    int progressBarMax = progressBar->maximum();
    int progressBarValue = progressBar->value();

    //Preparing logging

    log->append(stepsColor + tr("Beginning...") + endColor);
    printSettings();

    QFile fileLog("log.html");
    QFile debugFileLog("debugLog.html");

    QTextStream fileLogStream(&fileLog);
    QTextStream debugFileLogStream(&debugFileLog);

    fileLog.open(QFile::WriteOnly);
    debugFileLog.open(QFile::WriteOnly);

    fileLogStream << "<h1>" << QDateTime::currentDateTime().toString() << "</h1>";
    debugFileLogStream << "<h1>" << QDateTime::currentDateTime().toString() << "</h1>";

    //This loop applies the selected optimizations to each mod specified in ModDirs

    for(int i=0; i < modDirs.size(); ++i)
    {
        modPath = QDir::cleanPath(options.userPath + "/" + modDirs.at(i));
        modpathDir.setPath(modPath);
        QDirIterator it(modPath, QDirIterator::Subdirectories);

        debugLog->append(stepsColor + "[MAIN PROCESS FUNC]" + endColor + "ModDirs size: " + QString::number(modDirs.size()) + "\nCurrent index: " + QString::number(i));
        log->append(currentModColor + tr("Current mod: ") + modPath + endColor);

        meshesList();

        if(options.bBsaExtract)
        {
            bsaRename();
            bsaExtract();
            progressBar->setValue(progressBar->value() + 1);

        }

        debugLog->append(stepsColor + "Optimizing textures and meshes..." + endColor);

        while(it.hasNext())
        {
            progressBarValue = progressBar->value();
            progressBar->setMaximum(0); //This will show a busy indicator on the progress bar
            progressBar->setMinimum(0);

            it.next();
            //debugLog->append(noteColor + "Current file: " + it.filePath() + endColor);     Too much cluttering, will have to find a better way

            QCoreApplication::processEvents();

            if((options.bMeshesNecessaryOptimization || options.bMeshesMediumOptimization || options.bMeshesFullOptimization) && it.fileName().contains(".nif", Qt::CaseInsensitive))
            {
                meshesOptimize(&it);
                //textureCaseFixMesh(&it);    Currently not working, WIP
            }
            if((options.bTexturesFullOptimization) && it.fileName().contains(".dds", Qt::CaseInsensitive))
                texturesBc7Conversion(&it);

            if((options.bTexturesNecessaryOptimization) && it.fileName().contains(".tga", Qt::CaseInsensitive))
                texturesTgaToDds(&it);

            if(options.bAnimationsOptimization && it.fileName().contains(".hkx", Qt::CaseInsensitive))
                animationsOptimize(&it);
        }

        progressBar->setValue(progressBarValue + 1);
        progressBar->setMaximum(progressBarMax);

        if(options.bBsaCreate)
        {
            bsaCreate();
            progressBar->setValue(progressBar->value() + 1);
        }

        QCoreApplication::processEvents();

        debugFileLogStream << debugLog->toHtml();
        fileLogStream << log->toHtml();

    }

    //Deleting empty dirs

    system(QString("cd /d \"" + options.userPath + R"(" && for /f "delims=" %d in ('dir /s /b /ad ^| sort /r') do rd "%d" >nul 2>&1)").toStdString().c_str());

    progressBar->setValue(progressBar->maximum());
    log->append(stepsColor + tr("Completed. Please read the above text to check if any errors occurred (displayed in red).") + endColor);

    debugLog->append(stepsColor + "[/MAIN PROCESS FUNC]" + endColor);

    debugFileLogStream << debugLog->toHtml();
    fileLogStream << log->toHtml();

    return 0;
}


void Optimiser::dryRun() // Perform a dry run : list files without actually modifying them
{
    setup();

    log->append(tr("Beginning the dry run..."));

    for(int i=0; i < modDirs.size(); ++i)
    {
        modPath = QDir::cleanPath(options.userPath + "/" + modDirs.at(i));
        QDirIterator it(modPath, QDirIterator::Subdirectories);

        meshesList();

        log->append(currentModColor + tr("Current mod: ") + modPath + endColor);

        if(options.bBsaExtract)
        {
            bsaRename();
            bsaExtract();
            progressBar->setValue(progressBar->value() + 1);
        }

        while(it.hasNext())
        {
            it.next();

            QCoreApplication::processEvents();

            if(it.fileName().contains(".nif", Qt::CaseInsensitive))
            {
                if(options.bMeshesNecessaryOptimization && headparts.contains(it.filePath(), Qt::CaseInsensitive))
                    log->append(it.filePath() + tr(" would be optimized by Headparts meshes option"));

                else if(options.bMeshesMediumOptimization && otherMeshes.contains(it.filePath()))
                    log->append(it.filePath() + tr(" would be optimized lightly by the Other Meshes option"));

                else if(options.bMeshesNecessaryOptimization && crashingMeshes.contains(it.filePath(), Qt::CaseInsensitive))
                    log->append(it.filePath() + tr(" would be optimized in full by the Hard Crashing Meshes option.\n"));

                else if(options.bMeshesFullOptimization)
                    log->append(it.filePath() + tr(" would be optimized lightly by the Other Meshes option"));
            }
            if((options.bTexturesFullOptimization) && it.fileName().contains(".dds", Qt::CaseInsensitive))
            {
                QProcess texDiag;
                QStringList texdiagArg;

                texdiagArg << "info" << it.filePath();

                texDiag.start("resources/texdiag.exe", texdiagArg);
                texDiag.waitForFinished(-1);

                if(texDiag.readAllStandardOutput().contains("compressed = no"))
                {
                    log->append(it.filePath() + tr(" would be optimized using BC7 compression.\n"));
                }
            }

            if((options.bTexturesNecessaryOptimization) && it.fileName().contains(".tga", Qt::CaseInsensitive))
                log->append(it.filePath() + tr(" would be converted to DDS"));
        }

        progressBar->setValue(progressBar->value() + 1);

        QCoreApplication::processEvents();
    }

    progressBar->setValue(progressBar->maximum());
    log->append(stepsColor + tr("Completed.") + endColor);
}


void Optimiser::bsaExtract() //Extracts all BSA in modPath
{
    log->append(stepsColor + tr("Extracting BSA...") + endColor);

    QProcess bsarch;
    QStringList bsarchArgs;

    QStringList files(modpathDir.entryList());

    for (int i = 0; i < files.size(); ++i)
    {
        if(files.at(i).right(8) == ".bsa.bak")
        {
            log->append(tr("BSA found ! Extracting...(this may take a long time, do not force close the program): " + files.at(i).toLocal8Bit()));
            QCoreApplication::processEvents();

            QString bsaName = QDir::cleanPath(modPath + QDir::separator() + files.at(i));
            QString bsaFolder = QDir::cleanPath(modPath + QDir::separator() + files.at(i).chopped(4) + ".extracted");
            modpathDir.mkdir(bsaFolder);

            bsarchArgs.clear();
            bsarchArgs << "unpack" << bsaName << bsaFolder ;
            bsarch.start("resources/bsarch.exe", bsarchArgs);
            bsarch.waitForFinished(-1);

            debugLog->append(stepsColor + "[EXTRACT BSA FUNC]" + endColor + "BSArch Args :" + bsarchArgs.join(" ") + "\nBSA folder :" + bsaFolder + "\nBSA Name :" + bsaName + "\n" + stepsColor + "[/EXTRACT BSA FUNC]" + endColor);

            if(!bsarch.readAllStandardOutput().contains("Done"))
                log->append(errorColor + tr("An error occured during the extraction. Please extract it manually. The BSA was not deleted.") + endColor);
            else
            {
                if(!options.bBsaCreate)
                    moveAssets(bsaFolder, modPath, true);
            }
        }
        QCoreApplication::processEvents();
    }
}


void Optimiser::bsaRename() //Renames all BSA in modPath
//FIXME BSA are sometimes renamed even if they shouldn't
{
    log->append(stepsColor + tr("Renaming BSA...") + endColor);

    QDirIterator it(modPath);

    while (it.hasNext())
    {
        it.next();
        if(it.fileName().contains(".bsa.bak"))
        {
            QFile bakBsa(it.filePath());
            QFile currentBsa(it.filePath().chopped(4));
            if(currentBsa.exists() && !QFileInfo(currentBsa).isDir())
            {
                if(bakBsa.size() == currentBsa.size())
                    currentBsa.remove();
                else
                {
                    bakBsa.rename(bakBsa.fileName() + ".bak");
                    currentBsa.rename(currentBsa.fileName() + ".bak");
                }
            }
        }
        else if(it.fileName().contains(".bsa") && !it.fileInfo().isDir())
            QFile::rename(it.filePath(), it.filePath() + ".bak");
    }
}


void Optimiser::bsaCreate() //Once all the optimizations are done, create a new BSA
{
    log->append(stepsColor + tr("Creating a new BSA...(this may take a long time, do not force close the program)") + endColor);

    QStringList dirs(modpathDir.entryList(QDir::Dirs));
    QStringList FoldersToProcess;

    QStringList bsarchArgs;
    QString bsaName;
    QString espName = getPlugin();

    QDir bsaDir;
    QStringList bsaDirs;
    QProcess bsarch;

    bool hasSound=false;

    for (int i = 0; i < dirs.size(); ++i)
    {
        if(dirs.at(i).right(13) == "bsa.extracted")
            FoldersToProcess << QDir::cleanPath(modPath + "/" + dirs.at(i).mid(dirs.at(i).lastIndexOf("/")));
    }
    if(options.bBsaPackLooseFiles && FoldersToProcess.isEmpty())
    {
        QFile::copy("resources/BlankSSEPlugin.esp", modPath + "/" + espName);
        FoldersToProcess << QDir::cleanPath(modPath + "/" + espName.chopped(4) + ".bsa.extracted");
    }

    if(options.bBsaPackLooseFiles)
        moveAssets(modPath, FoldersToProcess.at(0), false);

    //Doing twice the same list since moveAssets() can create dirs

    FoldersToProcess.clear();
    dirs = modpathDir.entryList(QDir::Dirs);

    for (int i = 0; i < dirs.size(); ++i)
    {
        if(dirs.at(i).right(13) == "bsa.extracted")
            FoldersToProcess << QDir::cleanPath(modPath + "/" + dirs.at(i).mid(dirs.at(i).lastIndexOf("/")));
    }

    for (int i = 0; i < FoldersToProcess.size(); ++i)
    {
        bsarchArgs.clear();
        bsaDir.setPath(FoldersToProcess.at(i));
        bsaDirs = bsaDir.entryList(QDir::Dirs);

        //Detecting if BSA will contain sounds, since compressing BSA breaks sounds

        hasSound = false;
        for (int j = 0; j < bsaDirs.size(); ++j)
        {
            if(bsaDirs.at(j).toLower() == "sound" || bsaDirs.at(j).toLower() == "music")
                hasSound=true;
        }

        //Checking if it a textures BSA

        if(bsaDir.count() == 3 && bsaDirs.contains("TEXTURES") && !FoldersToProcess.at(i).contains("textures", Qt::CaseInsensitive))
            bsaName = FoldersToProcess.at(i).chopped(14) + " - Textures.bsa";
        else
            bsaName = FoldersToProcess.at(i).chopped(14) + ".bsa";

        bsarchArgs << "pack" << FoldersToProcess.at(i) << bsaName << "-sse" << "-share";

        if (!hasSound) //Compressing BSA breaks sounds
            bsarchArgs << "-z";

        if(!QFile(bsaName).exists())
        {
            bsarch.start("resources/bsarch.exe", bsarchArgs);
            bsarch.waitForFinished(-1);
        }
        else
        {
            log->append(errorColor + tr("Cannot pack existing loose files: a BSA already exists.") + endColor);
            moveAssets(FoldersToProcess.at(i), modPath, true);
        }

        debugLog->append(stepsColor + "[CREATE BSA FUNC]" + endColor + "BSArch Args :" + bsarchArgs.join(" ") + "\nBSA folder :" + FoldersToProcess.at(i) + "\nBsaName : " + bsaName + "\nBSAsize: " + QString::number(QFile(bsaName).size()) + "\n" + stepsColor + "[/CREATE BSA FUNC]" + endColor);

        if(bsarch.readAllStandardOutput().contains("Done"))
        {
            if(QFile(bsaName).size() < 2400000000)
            {
                log->append(tr("BSA successfully compressed: ") + bsaName + "\n");
                bsaDir.setPath(FoldersToProcess.at(i));
                bsaDir.removeRecursively();
                if(options.bBsaDeleteBackup)
                    QFile::remove(bsaName + ".bak");
            }
            else
            {
                log->append(errorColor + tr("The BSA was not compressed: it is over 2.2gb.") + endColor);
                QFile::remove(bsaName);
                if(QFile(modPath + "/" + espName).size() == QFile("resources/BlankSSEPlugin.esp").size())
                    QFile::remove(modPath + "/" + espName);
                moveAssets(FoldersToProcess.at(i), modPath, true);
            }

            if(QFile(bsaName).size() > 2147483648)
            {
                log->append(noteColor + "Warning: the BSA is nearly over its maximum size. It still should work." + endColor);
                if(options.bBsaDeleteBackup)
                    QFile::remove(bsaName + ".bak");
            }
        }
    }
    QCoreApplication::processEvents();
}



void Optimiser::texturesBc7Conversion(QDirIterator *it) //Compress uncompressed textures to BC7
{
    QProcess texDiag;
    QStringList texdiagArg;

    QProcess texconv;
    QStringList texconvArg;

    texdiagArg << "info" << it->filePath();

    texDiag.start("resources/texdiag.exe", texdiagArg);
    texDiag.waitForFinished(-1);

    QString texDiagOutput = texDiag.readAllStandardOutput();

    if(texDiagOutput.contains("compressed = no"))
    {
        QString width = texDiagOutput.mid(texDiagOutput.indexOf("width = ")+8, 4);
        QString height = texDiagOutput.mid(texDiagOutput.indexOf("height = ")+9, 4);
        int textureSize = width.simplified().toInt() * height.simplified().toInt();

        if(textureSize > 128*128)
        {
            texconvArg.clear();
            texconvArg << "-nologo" << "-y" << "-m" << "0" << "-pow2" << "-if" << "FANT" << "-f" << "BC7_UNORM" << it->filePath();

            log->append(tr("\nUncompressed texture found:\n") + it->fileName() + tr("\nCompressing..."));
            texconv.start("resources/texconv.exe",  texconvArg);
            texconv.waitForFinished(-1);
        }
    }
}


void Optimiser::texturesTgaToDds(QDirIterator* it) //Convert TGA textures to DDS
{
    log->append(stepsColor + tr("Converting TGA files...") + endColor);

    QProcess texconv;
    QStringList texconvArg;

    log->append(tr("\nTGA file found: \n") + it->fileName() + tr("\nCompressing..."));


    texconvArg.clear();
    texconvArg << "-nologo" << "-m" << "0" << "-pow2" << "-if" << "FANT" << "-f" << "R8G8B8A8_UNORM" << it->filePath();
    texconv.start("resources/texconv.exe",  texconvArg);
    texconv.waitForFinished(-1);

    QFile tga(it->filePath());
    tga.remove();

}


void Optimiser::meshesList() //Run NifScan on modPath. Detected meshes will be stored to a list, accorded to their types.
{
    log->append(stepsColor + tr("Running NifScan...") + endColor);
    QCoreApplication::processEvents();

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
    nifscanArgs << modPath << "-fixdds";

    nifScan.start("resources/NifScan.exe", nifscanArgs);

    if(!nifScan.waitForFinished(180000))
        log->append(errorColor + "Nifscan has not finished withing 3 minutes. Skipping mesh optimization for this mod." + endColor);

    while(nifScan.canReadLine())
    {
        readLine=QString::fromLocal8Bit(nifScan.readLine());

        if(readLine.contains("meshes\\", Qt::CaseInsensitive))
        {
            currentFile = readLine.simplified();
            if(currentFile.contains("facegendata"))
                headparts << QDir::cleanPath(modPath + "/" + currentFile);
            else
                otherMeshes << QDir::cleanPath(modPath + "/" + currentFile);
        }

        else if(readLine.contains("unsupported", Qt::CaseInsensitive) || readLine.contains("not supported", Qt::CaseInsensitive))
        {
            crashingMeshes << QDir::cleanPath(modPath + "/" + currentFile);
            otherMeshes.removeAll(QDir::cleanPath(modPath + "/" + currentFile));
        }
    }


    listHeadpartsArgs << modPath;
    listHeadparts.start("resources/ListHeadParts.exe", listHeadpartsArgs);
    listHeadparts.waitForFinished(-1);

    while(listHeadparts.canReadLine())
    {
        readLine=QString::fromLocal8Bit(listHeadparts.readLine());
        headparts << QDir::cleanPath(modPath + "/" + readLine.simplified());
    }

    //Adding custom headparts to detected headparts

    for(int i = 0; i < customHeadparts.size(); ++i)
    {
        headparts << QDir::cleanPath(modPath + "/" + customHeadparts.at(i));
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
}


void Optimiser::meshesOptimize(QDirIterator *it) // Optimize the selected mesh
{
    QProcess nifOpt;
    QStringList nifOptArgs;

    if(options.bMeshesNecessaryOptimization && headparts.contains(it->filePath(), Qt::CaseInsensitive) && options.bMeshesHeadparts)
    {
        crashingMeshes.removeAll(it->filePath());
        nifOptArgs << it->filePath() << "-head" << "1" << "-bsTriShape" << "1";
        log->append(stepsColor +  tr("Running NifOpt...") + endColor + tr("Processing: ") + it->filePath() + tr(" as an headpart due to crashing meshes option"));
    }

    else if(options.bMeshesNecessaryOptimization && crashingMeshes.contains(it->filePath(), Qt::CaseInsensitive))
    {
        nifOptArgs << it->filePath() << "-head" << "0" << "-bsTriShape" << "1";
        log->append(stepsColor +  tr("Running NifOpt...") + endColor + tr("Processing: ") + it->filePath() + tr(" due to crashing meshes option"));
    }

    else if(options.bMeshesFullOptimization && otherMeshes.contains(it->filePath(), Qt::CaseInsensitive))
    {
        nifOptArgs << it->filePath() << "-head" << "0" << "-bsTriShape" << "1";
        log->append(stepsColor +  tr("Running NifOpt...") + endColor + tr("Processing: ") + it->filePath() + tr(" due to all meshes option"));
    }

    else if(options.bMeshesMediumOptimization && otherMeshes.contains(it->filePath(), Qt::CaseInsensitive))
    {
        nifOptArgs << it->filePath() << "-head" << "0" << "-bsTriShape" << "0";
        log->append(stepsColor +  tr("Running NifOpt...") + endColor + tr("Processing: ") + it->filePath() + tr(" due to other meshes option"));
    }

    else if(options.bMeshesFullOptimization)
    {
        nifOptArgs << it->filePath() << "-head" << "0" << "-bsTriShape" << "1";
        log->append(stepsColor +  tr("Running NifOpt...") + endColor + tr("Processing: ") + it->filePath() + tr(" due to all meshes option"));
    }

    nifOpt.start("resources/nifopt.exe", nifOptArgs);
    nifOpt.waitForFinished(-1);
}


void Optimiser::meshesTexturesCaseFix(QDirIterator *it) //Unused. Work in progress. Same func as NIF Texcase Fixer
{
    QFile file(it->filePath());
    QString binaryData;
    QString texturePath;
    QStringList storedTextures;
    QVector <QStringRef> matches;

    QDirIterator textures(modPath, QDirIterator::Subdirectories);

    while (it->hasNext())
    {
        if(it->next().right(3).toLower() == "dds")
            storedTextures << modpathDir.relativeFilePath(textures.filePath());
    }

    file.open(QFile::ReadWrite);
    binaryData = QTextCodec::codecForMib(106)->toUnicode(file.read(999999));

    qDebug() << it->filePath();

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


void Optimiser::animationsOptimize(QDirIterator *it) //Run Bethesda Havok Tool to port the selected animation
{
    log->append(stepsColor + tr("Processing animations...") + endColor);

    QProcess havokProcess;
    QStringList havokArgs;

    log->append(tr("Current file: ") + it->filePath() + tr("\nProcessing...\n"));

    havokArgs.clear();
    havokArgs << "--platformamd64" << it->filePath() << it->filePath();
    havokProcess.start("resources/HavokBehaviorPostProcess.exe", havokArgs);

    havokProcess.waitForFinished(-1);

    if(havokProcess.readAllStandardOutput().isEmpty())
        log->append(stepsColor + tr("Animation successfully ported.") + endColor);
    else
        log->append(noteColor + tr("An error occured during the animation porting. Maybe it is already compatible with SSE ?") + endColor);
}


QString Optimiser::findSkyrimDirectory() //Find Skyrim directory using the registry key
{
    QSettings SkyrimReg(R"(HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Bethesda Softworks\Skyrim Special Edition)", QSettings::NativeFormat);
    QDir SkyrimDir = QDir::cleanPath(SkyrimReg.value("Installed Path").toString());
    return SkyrimDir.path();
}


QString Optimiser::getPlugin() //Find esp/esl/esm name using an iterator and regex. Also creates a plugin if there isn't one.
{
    QDirIterator it(modPath);
    QString espName;

    while (it.hasNext())
    {
        it.next();

        if(it.fileName().contains(QRegularExpression("\\.es[plm]")))
        {
            espName=it.fileName();
            log->append(tr("Esp found: ") + espName + "\n");
            return espName;
        }
    }

    espName = modPath.mid(modPath.lastIndexOf("/")+1) + ".esp";
    return espName;
}


void Optimiser::moveAssets(QString source, QString dest, bool allFiles) //Moves files reported in assets list to specified folder
{
    QStringList assets;
    QString relativeFilename;

    QDir sourceDir(source);
    QDirIterator it(source, QDirIterator::Subdirectories);

    QFile oldAsset;
    QFile newAsset;

    source = QDir::cleanPath(source) + "/";
    dest = QDir::cleanPath(dest) + "/";

    debugLog->append(stepsColor + "[MOVE ASSETS FUNC]" + endColor + "dest folder: " + dest + "\nsource folder: " + source + "\n allFiles " + allFiles);

    assets << "nif" << "seq" << "pex" << "psc" << "lod" << "fuz" << "waw" << "xwm" << "swf" << "hkx" << "wav" << "tri" << "btr" << "bto" << "btt" << "lip";

    sourceDir.mkdir(dest);

    while (it.hasNext())
    {
        it.next();
        if(it.fileName().right(3).toLower() == "dds" && it.path() != dest && !allFiles)
        {
            oldAsset.setFileName(it.filePath());
            relativeFilename = sourceDir.relativeFilePath(it.filePath());
            newAsset.setFileName(QString(dest).remove(".bsa.extracted/") + " - Textures.bsa.extracted/" + relativeFilename);

            if(newAsset.exists() && oldAsset.size() == newAsset.size())
                QFile::remove(newAsset.fileName());

            sourceDir.mkpath(newAsset.fileName().left(newAsset.fileName().lastIndexOf("/")));
            sourceDir.rename(oldAsset.fileName(), newAsset.fileName());
        }

        else if((assets.contains(it.fileName().right(3), Qt::CaseInsensitive) || allFiles) && it.path() != dest)
        {
            oldAsset.setFileName(it.filePath());
            relativeFilename = sourceDir.relativeFilePath(it.filePath());
            newAsset.setFileName(dest + relativeFilename);

            //removing the duplicate assets from bsa (if allFiles) or from loose files (if !allFiles)

            if(newAsset.exists() && oldAsset.size() == newAsset.size() && !allFiles)
                QFile::remove(newAsset.fileName());
            else if(newAsset.exists() && oldAsset.size() == newAsset.size() && allFiles)
                QFile::remove(oldAsset.fileName());

            sourceDir.mkpath(newAsset.fileName().left(newAsset.fileName().lastIndexOf("/")));
            sourceDir.rename(oldAsset.fileName(), newAsset.fileName());
        }
        QCoreApplication::processEvents();
    }
    system(qPrintable(QString("cd /d \"" + modPath + R"(" && for /f "delims=" %d in ('dir /s /b /ad ^| sort /r') do rd "%d" >nul 2>&1)"))); //Deleting empty folders
    debugLog->append(stepsColor + "[/MOVE ASSETS FUNC]" + endColor);

}


void Optimiser::saveSettings() //Saves settings to an ini file
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");


    settings.setValue("options.mode", options.mode);
    settings.setValue("DryRun", options.bDryRun);
    settings.setValue("SelectedPath", options.userPath);

    settings.setValue("bBsaExtract", options.bBsaExtract);
    settings.setValue("bBsaCreate", options.bBsaCreate);
    settings.setValue("bBsaPackLooseFiles", options.bBsaPackLooseFiles);
    settings.setValue("bBsaDeleteBackup", options.bBsaDeleteBackup);

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

    options.bBsaExtract = settings.value("bBsaExtract").toBool();
    options.bBsaCreate = settings.value("bBsaCreate").toBool();
    options.bBsaPackLooseFiles = settings.value("bBsaPackLooseFiles").toBool();
    options.bBsaDeleteBackup = settings.value("bBsaDeleteBackup").toBool();

    options.bMeshesNecessaryOptimization = settings.value("bMeshesNecessaryOptimization").toBool();
    options.bMeshesMediumOptimization = settings.value("bMeshesMediumOptimization").toBool();
    options.bMeshesFullOptimization = settings.value("bMeshesFullOptimization").toBool();

    options.bTexturesNecessaryOptimization = settings.value("bTexturesNecessaryOptimization").toBool();
    options.bTexturesFullOptimization = settings.value("bTexturesFullOptimization").toBool();

    options.bAnimationsOptimization = settings.value("bAnimationsOptimization").toBool();
}


void Optimiser::resetToDefaultSettings() //Reset to default (recommended) settings
{
    options.mode = 0;
    options.userPath = "";

    options.bBsaExtract = true;
    options.bBsaCreate = true;
    options.bBsaPackLooseFiles = false;
    options.bBsaDeleteBackup = false;

    options.bMeshesNecessaryOptimization = true;
    options.bMeshesMediumOptimization = false;
    options.bMeshesFullOptimization = false;

    options.bTexturesNecessaryOptimization = true;
    options.bTexturesFullOptimization = true;

    options.bAnimationsOptimization = true;

    options.bDryRun = false;
}


void Optimiser::printSettings() //Will print settings into debug log
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");

    debugLog->append("mode: " + QString::number(options.mode));
    debugLog->append("DryRun: " + QString::number(options.bDryRun));
    debugLog->append("userPath: "+ options.userPath);

    debugLog->append("bBsaExtract: "+ QString::number(options.bBsaExtract));
    debugLog->append("bBsaCreate: " + QString::number(options.bBsaCreate));
    debugLog->append("bBsaPackLooseFiles: " + QString::number(options.bBsaPackLooseFiles));
    debugLog->append("bBsaDeleteBackup: " + QString::number(options.bBsaDeleteBackup));

    debugLog->append("bMeshesNecessaryOptimization: "+ QString::number(options.bMeshesNecessaryOptimization));
    debugLog->append("bMeshesMediumOptimization: "+ QString::number(options.bMeshesMediumOptimization));
    debugLog->append("bMeshesFullOptimization: " + QString::number(options.bMeshesFullOptimization));

    debugLog->append("bTexturesNecessaryOptimization: "+ QString::number( options.bTexturesNecessaryOptimization));
    debugLog->append("bTexturesFullOptimization: " + QString::number( options.bTexturesFullOptimization));

    debugLog->append("bAnimationsOptimization: " + QString::number(options.bAnimationsOptimization));
}


void Optimiser::setDebugLog(QTextEdit *log) //Setter for debugLog
{
    debugLog = log;
}
