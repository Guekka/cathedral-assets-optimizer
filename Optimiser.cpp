#include "Optimiser.hpp"

const QString redColor("<font color=Red>");
const QString greenColor("<font color=Green>");
const QString greyColor("<font color=Grey>");
const QString orangeColor("<font color=Orange>");
const QString endColor("</font>\n");

Optimiser::Optimiser(QPlainTextEdit* textedit, QPlainTextEdit* debuglog, QProgressBar* bar) : log(textedit), debugLog(debuglog), progressBar(bar), dummyPluginsCounter(0) {}

Optimiser::~Optimiser()
{
    delete trash;
}


bool Optimiser::setup() //Some necessary operations before running
{
    saveSettings();

    modDirs.clear();
    progressBar->reset();
    log->clear();

    //Checking if all the requirements are in the resources folder

    QStringList requirements;
    QFile havokFile(findSkyrimDirectory() + "/Tools/HavokBehaviorPostProcess/HavokBehaviorPostProcess.exe");

    if(havokFile.exists() && !QFile("resources/HavokBehaviorPostProcess.exe").exists())
        havokFile.copy("resources/HavokBehaviorPostProcess.exe");

    else if(!havokFile.exists() && !QFile("resources/HavokBehaviorPostProcess.exe").exists())
        log->appendHtml(redColor + tr("Havok Tool not found. Are you sure the Creation Kit is installed ? You can also put HavokBehaviorPostProcess.exe in the resources folder") + endColor);

    requirements << "bsarch.exe" << "NifScan.exe" << "nifopt.exe" << "texconv.exe" << "texdiag.exe" << "HavokBehaviorPostProcess.exe";

    for (int i = 0; i < requirements.size(); ++i)
    {
        QFile file("resources/" + requirements.at(i));
        if(!file.exists())
        {
            log->appendHtml(redColor + requirements.at(i) + tr(" not found. Cancelling.") + endColor);
            return false;
        }
    }

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
    debugLog->appendPlainText("<SETUP FUNC>\nModpath: " + modPath + "\n</SETUP FUNC>");

    progressBar->setMaximum(modDirs.size()*(options.bExtractBsa + 1 + options.bCreateBsa));

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

    log->appendHtml(greenColor + tr("Beginning...") + endColor);

    int progressBarMax = progressBar->maximum();
    int progressBarValue = progressBar->value();

    //This loop applies the selected optimizations to each mod specified in ModDirs

    for(int i=0; i < modDirs.size(); ++i)
    {
        modPath = QDir::cleanPath(options.userPath + "/" + modDirs.at(i));
        QDirIterator it(modPath, QDirIterator::Subdirectories);

        debugLog->appendPlainText("<MAIN PROCESS FUNC>\nModDirs size: " + QString::number(modDirs.size()) + "\nCurrent index: " + QString::number(i));
        log->appendHtml(orangeColor + tr("Current mod: ") + modPath + endColor);

        listMeshes();

        if(options.bExtractBsa)
        {
            renameBsa();
            extractBsa();
            progressBar->setValue(progressBar->value() + 1);
        }

        if(options.bNifscanOnTextures)
            nifscanTextures();

        while(it.hasNext())
        {
            progressBarValue = progressBar->value();
            progressBar->setMaximum(0); //This will show a busy indicator on the progress bar
            progressBar->setMinimum(0);

            it.next();
            QCoreApplication::processEvents();

            if((options.bOptimizeHardCrashingMeshes || options.bOptimizeOtherMeshes) && it.fileName().contains(".nif", Qt::CaseInsensitive))
                optimizeMesh(&it);

            if((options.bBc7Conversion) && it.fileName().contains(".dds", Qt::CaseInsensitive))
                bc7TexturesConversion(&it);

            if((options.bTgaConversion) && it.fileName().contains(".tga", Qt::CaseInsensitive))
                tgaToDdsConversion(&it);

            if(options.bOptimizeAnimations && it.fileName().contains(".hkx", Qt::CaseInsensitive))
                optimizeAnimation(&it);
        }

        progressBar->setValue(progressBarValue + 1);
        progressBar->setMaximum(progressBarMax);

        if(options.bCreateBsa)
        {
            createBsa();
            progressBar->setValue(progressBar->value() + 1);
        }

        QCoreApplication::processEvents();
    }

    //Deleting empty dirs

    system(QString("cd /d \"" + options.userPath + R"(" && for /f "delims=" %d in ('dir /s /b /ad ^| sort /r') do rd "%d" >nul 2>&1)").toStdString().c_str());

    progressBar->setValue(progressBar->maximum());
    log->appendHtml(greenColor + tr("Completed. Please read the above text to check if any errors occurred (displayed in red).") + endColor);

    debugLog->appendPlainText("\n</MAIN PROCESS FUNC>");
    return 0;
}


void Optimiser::dryRun() // Perform a dry run : list files without actually modifying them
{
    setup();

    log->appendHtml(tr("Beginning the dry run..."));

    for(int i=0; i < modDirs.size(); ++i)
    {
        modPath = QDir::cleanPath(options.userPath + "/" + modDirs.at(i));
        QDirIterator it(modPath, QDirIterator::Subdirectories);

        listMeshes();

        log->appendHtml(orangeColor + tr("Current mod: ") + modPath + endColor);

        if(options.bExtractBsa)
        {
            renameBsa();
            extractBsa();
            progressBar->setValue(progressBar->value() + 1);
        }

        while(it.hasNext())
        {
            it.next();

            QCoreApplication::processEvents();

            if((options.bOptimizeHardCrashingMeshes || options.bOptimizeOtherMeshes) && it.fileName().contains(".nif", Qt::CaseInsensitive))
            {

                if(options.bOptimizeHardCrashingMeshes && !hardCrashingMeshes.contains(it.filePath(), Qt::CaseInsensitive) && it.filePath().contains("facegendata", Qt::CaseInsensitive))
                {
                    log->appendPlainText(it.filePath() + tr(" would be optimized by Headparts meshes option.\n"));
                }

                else if(options.bOptimizeOtherMeshes && !hardCrashingMeshes.contains(it.filePath(), Qt::CaseInsensitive) && otherMeshes.contains(it.filePath()))
                {
                    log->appendPlainText(it.filePath() + tr(" would be optimized lightly by the Other Meshes option.\n"));
                }

                else if(hardCrashingMeshes.contains(it.filePath(), Qt::CaseInsensitive))
                {
                    log->appendPlainText(it.filePath() + tr(" would be optimized in full by the Hard Crashing Meshes option.\n"));
                }

            }
            if((options.bBc7Conversion) && it.fileName().contains(".dds", Qt::CaseInsensitive))
            {
                QProcess texDiag;
                QStringList texdiagArg;

                texdiagArg << "info" << it.filePath();

                texDiag.start("resources/texdiag.exe", texdiagArg);
                texDiag.waitForFinished(-1);

                if(texDiag.readAllStandardOutput().contains("compressed = no"))
                {
                    log->appendPlainText(it.filePath() + tr(" would be optimized using BC7 compression.\n"));
                }
            }

            if((options.bTgaConversion) && it.fileName().contains(".tga", Qt::CaseInsensitive))
            {
                log->appendPlainText(it.filePath() + tr(" would be converted to DDS"));
            }
        }

        progressBar->setValue(progressBar->value() + 1);

        QCoreApplication::processEvents();
    }

    progressBar->setValue(progressBar->maximum());
    log->appendHtml(greenColor + tr("Completed.") + endColor);

}


void Optimiser::extractBsa() //Extracts all BSA in modPath
{
    log->appendHtml(greenColor + tr("Extracting BSA...") + endColor);

    QProcess bsarch;
    QStringList bsarchArgs;

    QDir modPathDir(modPath);
    QStringList files(modPathDir.entryList());

    for (int i = 0; i < files.size(); ++i)
    {
        if(files.at(i).contains(".bsa.bak"))
        {
            log->appendPlainText(tr("BSA found ! Extracting...(this may take a long time, do not force close the program): " + files.at(i).toLocal8Bit()));

            QString bsaName = QDir::cleanPath(modPath + QDir::separator() + files.at(i));
            QString bsaFolder = QDir::cleanPath(modPath + QDir::separator() + files.at(i).chopped(4) + ".extracted");
            modPathDir.mkdir(bsaFolder);

            bsarchArgs.clear();
            bsarchArgs << "unpack" << bsaName << bsaFolder ;
            bsarch.start("resources/bsarch.exe", bsarchArgs);
            bsarch.waitForFinished(-1);

            debugLog->appendPlainText("<EXTRACT BSA FUNC>\nBSArch Args :" + bsarchArgs.join(" ") + "\nBSA folder :" + bsaFolder + "\nBSA Name :" + bsaName + "\n</EXTRACT BSA FUNC>\n\n\n\n\n");

            if(!bsarch.readAllStandardOutput().contains("Done"))
                log->appendHtml(redColor + tr("An error occured during the extraction. Please extract it manually. The BSA was not deleted.") + endColor);
        }
        QCoreApplication::processEvents();
    }
}


void Optimiser::renameBsa() //Renames all BSA in modPath
{
    log->appendHtml(greenColor + tr("Renaming BSA...") + endColor);

    QDirIterator it(modPath);

    while (it.hasNext())
    {
        if(it.next().contains(".bsa.bak"))
        {
            QFile bakBsa(it.filePath());
            QFile currentBsa(it.filePath().chopped(4));
            if(currentBsa.exists() && !QFileInfo(currentBsa).isDir())
            {
                if(bakBsa.size() == currentBsa.size())
                {
                    currentBsa.remove();
                }
                else
                {
                    bakBsa.rename(bakBsa.fileName() + ".bak");
                    currentBsa.rename(currentBsa.fileName() + ".bak");
                }
            }
        }
        else if(it.fileName().contains(".bsa") && !it.fileInfo().isDir())
        {
            QFile::rename(it.filePath(), it.filePath() + ".bak");
        }
    }
}


void Optimiser::createBsa() //Once all the optimizations are done, create a new BSA
{
    log->appendHtml(greenColor + tr("Creating a new BSA...(this may take a long time, do not force close the program)") + endColor);

    QDir modPathDir(modPath);
    QStringList dirs(modPathDir.entryList(QDir::Dirs));

    QStringList bsarchArgs;
    QString folderName;
    QString bsaName;

    QDir bsaDir;
    QStringList bsaDirs;
    QProcess bsarch;

    bool hasSound=false;

    for (int i = 0; i < dirs.size(); ++i)
    {
        bsarchArgs.clear();
        if(dirs.at(i).right(13) == "bsa.extracted")
            folderName = modPath + "/" + dirs.at(i).mid(dirs.at(i).lastIndexOf("/"));

        else if(options.bPackExistingFiles)
        {
            folderName = modPath + "/" + findEspName().chopped(4) + ".bsa.extracted";
            moveAssets(folderName);
        }
        else
            folderName.clear();

        if(!folderName.isEmpty())
        {
            bsaDir.setPath(folderName);
            bsaDirs = bsaDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

            //Detecting if BSA will contain sounds, since compressing sounds breaks them

            for (int j = 0; j < bsaDirs.size(); ++j)
            {
                if(bsaDirs.at(j).toLower() == "sound" || bsaDirs.at(j).toLower() == "music")
                    hasSound=true;
            }


            //Checking if it a textures BSA

            if(bsaDir.count() == 1 && bsaDirs.at(0).toLower() == "textures" && !folderName.contains("textures", Qt::CaseInsensitive))
            {
                bsaName = folderName.chopped(14) + " - Textures.bsa";
            }
            else
            {
                bsaName = folderName.chopped(14) + ".bsa";
            }

            bsarchArgs << "pack" << folderName << bsaName << "-sse" << "-share";

            if (!hasSound) //Compressing BSA breaks sounds
            {
                bsarchArgs << "-z";
            }

            modPathDir.rename(folderName + "/meshes/actors/character/animations", "meshes/actors/character/animations"); //Moving animations because FNIS can't see them in BSA

            bsarch.start("resources/bsarch.exe", bsarchArgs);
            bsarch.waitForFinished(-1);

            debugLog->appendPlainText("<CREATE BSA FUNC>\nBSArch Args :" + bsarchArgs.join(" ") + "\nBSA folder :" + folderName + "\nBsaName : " + bsaName + "\nBSAsize: " + QString::number(QFile(bsaName).size()) + "\n</CREATE BSA FUNC>\n\n\n\n\n");

            if(bsarch.readAllStandardOutput().contains("Done"))
            {
                if(QFile(bsaName).size() < 2400000000)
                {
                    log->appendHtml(tr("BSA successfully compressed: ") + bsaName + "\n");
                    modPathDir.setPath(folderName);
                    modPathDir.removeRecursively();
                    modPathDir.rename(bsaName, folderName.chopped(10));
                }
                else
                {
                    log->appendHtml(redColor + "The BSA was not compressed: it is over 2.2gb" + endColor);
                    QFile::remove(bsaName);
                }

                if(QFile(bsaName).size() > 2147483648)
                {
                    log->appendHtml(greyColor + "Warning: the BSA is nearly over its maximum size. It still should work." + endColor);
                }
            }
        }
        QCoreApplication::processEvents();
    }
}


void Optimiser::bc7TexturesConversion(QDirIterator *it) //Compress uncompressed textures to BC7
{
    QProcess texDiag;
    QStringList texdiagArg;

    QProcess texconv;
    QStringList texconvArg;

    texdiagArg << "info" << it->filePath();

    texDiag.start("resources/texdiag.exe", texdiagArg);
    texDiag.waitForFinished(-1);

    if(texDiag.readAllStandardOutput().contains("compressed = no"))
    {
        texconvArg.clear();
        texconvArg << "-nologo" << "-y" << "-m" << "0" << "-pow2" << "-if" << "FANT" << "-f" << "BC7_UNORM" << it->filePath();

        log->appendPlainText(tr("\nUncompressed texture found:\n") + it->fileName() + tr("\nCompressing..."));
        texconv.start("resources/texconv.exe",  texconvArg);
        texconv.waitForFinished(-1);
    }
}


void Optimiser::nifscanTextures() // Runs Nifscan with -fixdds option. Will convert unsupported textures to a supported format.
{
    log->appendHtml(greenColor + tr("Running Nifscan on the textures...") + endColor);

    QFile nifScan_file("resources/NifScan.exe");
    QProcess nifScan;
    QStringList NifscanArgs;

    NifscanArgs << "-fixdds";

    QFile::copy("resources/NifScan.exe", modPath + "/NifScan.exe");
    nifScan.setWorkingDirectory(modPath);

    nifScan.start(modPath + "/NifScan.exe", NifscanArgs);
    nifScan.waitForFinished(-1);

    QFile::remove(modPath + "/NifScan.exe");

}


void Optimiser::tgaToDdsConversion(QDirIterator* it) //Convert TGA textures to DDS
{
    log->appendHtml(greenColor + tr("Converting TGA files...") + endColor);

    QProcess texconv;
    QStringList texconvArg;

    log->appendPlainText(tr("\nTGA file found: \n") + it->fileName() + tr("\nCompressing..."));


    texconvArg.clear();
    texconvArg << "-nologo" << "-m" << "0" << "-pow2" << "-if" << "FANT" << "-f" << "R8G8B8A8_UNORM" << it->filePath();
    texconv.start("resources/texconv.exe",  texconvArg);
    texconv.waitForFinished(-1);

    QFile tga(it->filePath());
    tga.remove();

}


void Optimiser::listMeshes() //Run NifScan on modPath. Detected meshes will be stored to a list, accorded to their types.
{
    log->appendHtml(greenColor + tr("Running NifScan...") + endColor);

    otherMeshes.clear();
    hardCrashingMeshes.clear();
    otherHeadparts.clear();

    QString readLine;
    QString currentFile;

    QFile nifScan_file("resources/NifScan.exe");

    QProcess nifScan;
    QProcess listHeadparts;

    QStringList listHeadpartsArgs;

    QFile::copy("resources/NifScan.exe", modPath + "/NifScan.exe");
    nifScan.setReadChannel(QProcess::StandardOutput);
    nifScan.setProgram(modPath + "/NifScan.exe");
    nifScan.setWorkingDirectory(modPath);

    nifScan.start();
    nifScan.waitForFinished(-1);

    while(nifScan.canReadLine())
    {
        readLine=QString::fromLocal8Bit(nifScan.readLine());

        if(readLine.contains("meshes\\", Qt::CaseInsensitive))
        {
            currentFile = readLine;
            otherMeshes << QDir::cleanPath(modPath + "/" + currentFile.simplified());
        }

        else if(readLine.contains("unsupported", Qt::CaseInsensitive) || readLine.contains("not supported", Qt::CaseInsensitive))
        {
            hardCrashingMeshes << QDir::cleanPath(modPath + "/" + currentFile.simplified());
        }
    }


    QFile::remove(modPath + "/NifScan.exe");


    listHeadpartsArgs << modPath;
    listHeadparts.start("resources/ListHeadParts.exe", listHeadpartsArgs);
    listHeadparts.waitForFinished(-1);

    while(listHeadparts.canReadLine())
    {
        readLine=QString::fromLocal8Bit(listHeadparts.readLine());
        otherHeadparts << QDir::cleanPath(modPath + "/" + readLine.simplified());
    }


    //Cleaning the lists

    otherHeadparts.removeDuplicates();
    otherMeshes.removeDuplicates();
    crashingHeadparts.removeDuplicates();
    hardCrashingMeshes.removeDuplicates();

    otherHeadparts.removeAll(QString(""));
    otherMeshes.removeAll(QString(""));
    crashingHeadparts.removeAll(QString(""));
    hardCrashingMeshes.removeAll(QString(""));


    //Removing hard crashing meshes from other meshes list

    QStringListIterator it(hardCrashingMeshes);

    while (it.hasNext())
    {
        otherMeshes.removeAll(it.next());
    }

    QStringListIterator it2(otherHeadparts);
    QString temp;

    while(it2.hasNext())
    {
        temp = it2.next();
        if(hardCrashingMeshes.contains(temp, Qt::CaseInsensitive))
        {
            crashingHeadparts << temp;
            otherHeadparts.removeAll(temp);
            hardCrashingMeshes.removeAll(temp);
        }
    }
}


void Optimiser::optimizeMesh(QDirIterator *it) // Optimize the selected mesh
{
    log->appendHtml(greenColor +  tr("Running NifOpt...") + endColor);
    log->appendPlainText(tr("Processing: ") + it->filePath());


    QProcess nifOpt;
    QStringList nifOptArgs;


    if(options.bOptimizeHardCrashingMeshes && crashingHeadparts.contains(it->filePath(), Qt::CaseInsensitive))
    {
        hardCrashingMeshes.removeAll(it->filePath());
        nifOptArgs.clear();
        nifOptArgs << it->filePath() << "-head" << "1" << "-bsTriShape" << "1";
    }

    else if (options.bOptimizeOtherMeshes && otherHeadparts.contains(it->filePath(), Qt::CaseInsensitive))
    {
        nifOptArgs.clear();
        nifOptArgs << it->filePath() << "-head" << "1" << "-bsTriShape" << "1";
    }

    else if(options.bOptimizeOtherMeshes && otherMeshes.contains(it->filePath()))
    {
        nifOptArgs.clear();
        nifOptArgs << it->filePath() << "-head" << "0" << "-bsTriShape" << "0";
    }

    else if(options.bOptimizeHardCrashingMeshes && hardCrashingMeshes.contains(it->filePath(), Qt::CaseInsensitive))
    {
        nifOptArgs.clear();
        nifOptArgs << it->filePath() << "-head" << "0" << "-bsTriShape" << "1";
    }

    nifOpt.start("resources/nifopt.exe", nifOptArgs);
    nifOpt.waitForFinished(-1);
}


void Optimiser::optimizeAnimation(QDirIterator *it) //Run Bethesda Havok Tool to port the selected animation
{
    log->appendHtml(greenColor + tr("Processing animations...") + endColor);

    QProcess havokProcess;
    QStringList havokArgs;

    log->appendPlainText(tr("Current file: ") + it->filePath() + tr("\nProcessing...\n"));

    havokArgs.clear();
    havokArgs << "--platformamd64" << it->filePath() << it->filePath();
    havokProcess.start("resources/HavokBehaviorPostProcess.exe", havokArgs);

    havokProcess.waitForFinished(-1);

    if(havokProcess.readAllStandardOutput().isEmpty())
        log->appendHtml(greenColor + tr("Animation successfully ported.") + endColor);
    else
        log->appendHtml(greyColor + tr("An error occured during the animation porting. Maybe it is already compatible with SSE ?") + endColor);
}


QString Optimiser::findSkyrimDirectory() //Find Skyrim directory using the registry key
{
    QSettings SkyrimReg(R"(HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Bethesda Softworks\Skyrim Special Edition)", QSettings::NativeFormat);
    QDir SkyrimDir = QDir::cleanPath(SkyrimReg.value("Installed Path").toString());
    return SkyrimDir.path();
}


QString Optimiser::findEspName() //Find esp/esl/esm name using an iterator and regex
{
    QDirIterator it(modPath);
    QString espName;

    while (it.hasNext())
    {
        it.next();

        if(it.fileName().contains(QRegularExpression("\\.es[plm]")))
        {
            espName=it.fileName();
            log->appendPlainText(tr("Esp found: ") + espName + "\n");
            return espName;
        }
    }
    ++dummyPluginsCounter;
    debugLog->appendPlainText("dummy_plugin" + QString::number(dummyPluginsCounter) + ".esp");
    return "dummy_plugin" + QString::number(dummyPluginsCounter) + ".esp";
}


void Optimiser::moveAssets(QString dest) //Moves files reported in assets list to specified folder
{
    QStringList assets;
    QDir modPathDir(modPath);
    QDirIterator it(modPath, QDirIterator::Subdirectories);
    QDirIterator firstIt(modPath);
    QFile looseAsset, bsaAsset;
    QString relativeFilename;
    dest = QDir::cleanPath(dest) + "/";

    debugLog->appendPlainText("\n<MOVE ASSETS FUNC>\ndest folder: " + dest);

    assets << "nif" << "seq" << "pex" << "psc" << "lod" << "fuz" << "waw" << "xwm" << "swf" << "txt" << "hkx" << "dds" << "wav" << "tri";

    modPathDir.mkdir(dest);

    while (firstIt.hasNext())
    {
        firstIt.next();
        if(firstIt.fileName().toLower() == "textures")
            modPathDir.rename(firstIt.fileName(), "TEXTURES"); //Making textures folder easily distinguishable
    }

    while (it.hasNext())
    {
        it.next();
        if(it.filePath().contains("TEXTURES") && !it.filePath().contains(".bsa", Qt::CaseInsensitive))
        {
            looseAsset.setFileName(it.filePath());
            relativeFilename = modPathDir.relativeFilePath(it.filePath());
            bsaAsset.setFileName(dest.chopped(15) + " - Textures.bsa/" + relativeFilename);

            if(bsaAsset.exists() && looseAsset.size() == bsaAsset.size())
                QFile::remove(bsaAsset.fileName());

            modPathDir.mkpath(bsaAsset.fileName().left(bsaAsset.fileName().lastIndexOf("/")));
            modPathDir.rename(looseAsset.fileName(), bsaAsset.fileName());
        }

        else if(!dest.contains(it.filePath()) && assets.contains(it.fileName().right(3), Qt::CaseInsensitive) && !it.filePath().contains(".bsa", Qt::CaseInsensitive))
        {
            looseAsset.setFileName(it.filePath());
            relativeFilename = modPathDir.relativeFilePath(it.filePath());
            bsaAsset.setFileName(dest + relativeFilename);

            if(bsaAsset.exists() && looseAsset.size() == bsaAsset.size())
                QFile::remove(bsaAsset.fileName());

            modPathDir.mkpath(bsaAsset.fileName().left(bsaAsset.fileName().lastIndexOf("/")));
            modPathDir.rename(looseAsset.fileName(), bsaAsset.fileName());
        }
        QCoreApplication::processEvents();
    }
    system(QString("cd /d \"" + modPath + R"(" && for /f "delims=" %d in ('dir /s /b /ad ^| sort /r') do rd "%d" >nul 2>&1)").toStdString().c_str()); //Deleting empty folders
    debugLog->appendPlainText("\n</MOVE ASSETS FUNC>\n");

}


void Optimiser::saveSettings() //Saves settings to an ini file
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");

    settings.setValue("options.mode", options.mode);
    settings.setValue("DryRun", options.bDryRun);
    settings.setValue("SelectedPath", options.userPath);

    settings.setValue("extractBSA", options.bExtractBsa);
    settings.setValue("recreateBSA", options.bCreateBsa);
    settings.setValue("PackExistingFiles", options.bPackExistingFiles);

    settings.setValue("HardCrashingMeshes", options.bOptimizeHardCrashingMeshes);
    settings.setValue("OtherMeshes", options.bOptimizeOtherMeshes);

    settings.setValue("TGAConv", options.bTgaConversion);
    settings.setValue("BC7Conv", options.bBc7Conversion);
    settings.setValue("nifscanTextures", options.bNifscanOnTextures);

    settings.setValue("AnimOpt", options.bOptimizeAnimations);
}


void Optimiser::loadSettings() //Loads settings from the ini file
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");

    options.mode = settings.value("options.mode").toInt();
    options.bDryRun = settings.value("DryRun").toBool();
    options.userPath = settings.value("SelectedPath").toString();

    options.bExtractBsa = settings.value("extractBSA").toBool();
    options.bCreateBsa = settings.value("recreateBSA").toBool();
    options.bPackExistingFiles = settings.value("PackExistingFiles").toBool();

    options.bOptimizeHardCrashingMeshes = settings.value("HardCrashingMeshes").toBool();
    options.bOptimizeOtherMeshes = settings.value("OtherMeshes").toBool();

    options.bTgaConversion = settings.value("TGAConv").toBool();
    options.bBc7Conversion = settings.value("BC7Conv").toBool();
    options.bNifscanOnTextures = settings.value("nifscanTextures").toBool();

    options.bOptimizeAnimations = settings.value("AnimOpt").toBool();
}


void Optimiser::resetToDefaultSettings() //Reset to default (recommended) settings
{
    options.mode = 0;
    options.userPath = "";

    options.bExtractBsa = true;
    options.bCreateBsa = true;
    options.bPackExistingFiles = false;

    options.bOptimizeHardCrashingMeshes = true;
    options.bOptimizeOtherMeshes = false;

    options.bTgaConversion = true;
    options.bBc7Conversion = true;
    options.bNifscanOnTextures = true;

    options.bOptimizeAnimations = true;

    options.bDryRun = false;
}


void Optimiser::printSettings() //Used by debug UI
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");


    debugLog->appendPlainText("mode: : " + QString::number(options.mode));
    debugLog->appendPlainText("DryRun: " + QString::number(options.bDryRun));
    debugLog->appendPlainText("SelectedPath: "+ options.userPath);

    debugLog->appendPlainText("Extract BSA: "+ QString::number(options.bExtractBsa));
    debugLog->appendPlainText("Recreate BSA: " + QString::number(options.bCreateBsa));
    debugLog->appendPlainText("PackExistingFiles: "+ QString::number(options.bPackExistingFiles));

    debugLog->appendPlainText("HardCrashingMeshes: "+ QString::number(options.bOptimizeHardCrashingMeshes));
    debugLog->appendPlainText("OtherMeshes: "+ QString::number(options.bOptimizeOtherMeshes));

    debugLog->appendPlainText("TGAConv: "+ QString::number( options.bTgaConversion));
    debugLog->appendPlainText("BC7Conv: " + QString::number( options.bBc7Conversion));
    debugLog->appendPlainText("nifscanTextures: " + QString::number( options.bNifscanOnTextures));

    debugLog->appendPlainText("AnimOpt: " + QString::number(options.bOptimizeAnimations));
}


void Optimiser::setDebugLog(QPlainTextEdit *log) //Setter for debugLog
{
    debugLog = log;
}
