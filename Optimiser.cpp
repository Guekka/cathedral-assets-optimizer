#include "Optimiser.hpp"


Optimiser::Optimiser(QPlainTextEdit* textEdit, QProgressBar* bar) : log(textEdit), progressBar(bar){}


int Optimiser::mainProcess() // Process the userPath according to all user options
{
    saveSettings();
    progressBar->reset();

    log->clear();

    QStringList modDirs;
    QStringList requirements;

    QFile havokFile(findSkyrimDir() + "/Tools/HavokBehaviorPostProcess/HavokBehaviorPostProcess.exe");

    if(havokFile.exists() && !QFile("resources/HavokBehaviorPostProcess.exe").exists())
    {
        havokFile.copy("resources/HavokBehaviorPostProcess.exe");
    }
    else if(!havokFile.exists() && !QFile("resources/HavokBehaviorPostProcess.exe").exists())
    {
        log->appendHtml(tr("<font color=Red>Havok Tool not found. Are you sure the Creation Kit is installed ? You can also put HavokBehaviorPostProcess.exe in the resources folder.</font>\n"));
        return 1;
    }


    requirements << "bsarch.exe" << "NifScan.exe" << "nifopt.exe" << "texconv.exe" << "texdiag.exe" << "HavokBehaviorPostProcess.exe";

    for (int i = 0; i < requirements.size(); ++i)
    {
        QFile file("resources/" + requirements.at(i));
        if(!file.exists())
        {
            log->appendHtml("<font color=Red>" + requirements.at(i) + tr(" not found. Cancelling."));
            return 1;
        }
    }

    if(options.mode == 1)
    {
        QDir dir(options.userPath);
        dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
        modDirs = dir.entryList();
    }

    if(options.mode == 0)
    {
        modDirs << ""; //if modDirs is empty, the mod won't be processed
    }


    progressBar->setMaximum(modDirs.size()*(options.extractBsa + 1 + (options.recreateBsa * 2)));

    if(options.dryRun)
    {
        dryRun();
        return 2;
    }

    log->appendHtml(tr("<font color=Blue>Beginning...</font>"));

    for(int i=0; i < modDirs.size(); ++i)
    {
        modPath = QDir::cleanPath(options.userPath + "/" + modDirs.at(i));
        QDirIterator it(modPath, QDirIterator::Subdirectories);

        nifScan();
        moveToTemp();

        log->appendHtml("<font color=Orange>" + tr("Current mod: ") + modPath + "</font>");

        if(options.extractBsa)
        {
            extractBsa();
            progressBar->setValue(progressBar->value() + 1);
        }


        if(options.nifscanTextures)
        {
            nifscanTextures();
        }

        while(it.hasNext())
        {
            it.next();
            QCoreApplication::processEvents();

            if((options.hardCrashingMeshes || options.otherMeshes) && it.fileName().contains(".nif", Qt::CaseInsensitive))
                nifOpt(&it);

            if((options.bc7Conv) && it.fileName().contains(".dds", Qt::CaseInsensitive))
                bc7Conv(&it);

            if((options.tgaConv) && it.fileName().contains(".tga", Qt::CaseInsensitive))
                tgaConv(&it);

            if(options.animOptBool && it.fileName().contains(".hkx", Qt::CaseInsensitive))
                animOpt(&it);

        }

        progressBar->setValue(progressBar->value() + 1);
        deleteEmptyDirs(modPath);

        if(options.recreateBsa)
        {
            createBsa();
            progressBar->setValue(progressBar->value() + 1);
            createTexturesBsa();
            progressBar->setValue(progressBar->value() + 1);
        }

        restoreFromTemp(true, true);

        QCoreApplication::processEvents();
    }

    progressBar->setValue(progressBar->maximum());
    log->appendHtml(tr("<font color=blue>Completed. Please read the above text to check if any errors occurred (displayed in red).</font>\n"));


    return 0;
}


void Optimiser::dryRun()
{
    log->appendHtml(tr("Beginning the dry run..."));

    QStringList modDirs;

    if(options.mode == 1)
    {
        QDir dir(options.userPath);
        dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
        modDirs = dir.entryList();
    }

    if(options.mode == 0)
    {
        modDirs << "";
    }

    progressBar->setMaximum(modDirs.size()*6);

    for(int i=0; i < modDirs.size(); ++i)
    {
        modPath = QDir::cleanPath(options.userPath + "/" + modDirs.at(i));
        QDirIterator it(modPath, QDirIterator::Subdirectories);

        nifScan();

        log->appendHtml("<font color=Orange>" + tr("Current mod: ") + modPath + "</font>");

        if(options.extractBsa)
        {
            extractBsa();
            progressBar->setValue(progressBar->value() + 1);
        }


        while(it.hasNext())
        {
            it.next();


            QCoreApplication::processEvents();

            if((options.hardCrashingMeshes || options.otherMeshes) && it.fileName().contains(".nif", Qt::CaseInsensitive))
            {

                if(options.hardCrashingMeshes && !hardCrashingMeshes.contains(it.filePath(), Qt::CaseInsensitive) && it.filePath().contains("facegendata", Qt::CaseInsensitive))
                {
                    log->appendPlainText(it.filePath() + tr(" would be optimized by Headparts meshes option.\n"));
                }

                else if(options.otherMeshes && !hardCrashingMeshes.contains(it.filePath(), Qt::CaseInsensitive) && otherMeshes.contains(it.filePath()))
                {
                    log->appendPlainText(it.filePath() + tr(" would be optimized lightly by the Other Meshes option.\n"));
                }

                else if(hardCrashingMeshes.contains(it.filePath(), Qt::CaseInsensitive))
                {
                    log->appendPlainText(it.filePath() + tr(" would be optimized in full by the Hard Crashing Meshes option.\n"));
                }

            }
            if((options.bc7Conv) && it.fileName().contains(".dds", Qt::CaseInsensitive))
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

            if((options.tgaConv) && it.fileName().contains(".tga", Qt::CaseInsensitive))
            {
                log->appendPlainText(it.filePath() + tr(" would be converted to DDS"));
            }
        }

        progressBar->setValue(progressBar->value() + 1);

        QCoreApplication::processEvents();
        restoreFromTemp(true, true);
    }

    progressBar->setValue(progressBar->maximum());
    log->appendHtml(tr("<font color=blue>Completed.</font>\n"));

}



void Optimiser::extractBsa() //Extracts the BSA
{
    log->appendHtml(tr("\n\n\n<font color=Blue>Extracting BSA..."));


    QProcess bsarch;
    QStringList bsarchArgs;

    QDir modPathDir(modPath);

    QDirIterator it(modPath);

    renameBsa();

    //    moveToTemp();

    while (it.hasNext())
    {
        if(it.next().contains(".bsa.bak"))
        {
            log->appendPlainText(tr("BSA found ! Extracting..."));


            QString bsaFolder = modPath + QDir::separator() + it.fileName().chopped(4);
            modPathDir.mkdir(bsaFolder);

            bsarchArgs.clear();
            bsarchArgs << "unpack" << it.filePath() << bsaFolder ;
            bsarch.start("resources/bsarch.exe", bsarchArgs);
            bsarch.waitForFinished(-1);

            if(bsarch.readAllStandardOutput().contains("Done."))
            {
                log->appendHtml(tr("<font color=Blue>BSA successfully extracted.</font>\n"));

            }
            else{
                log->appendHtml(tr("<font color=Red>An error occured during the extraction. Please extract it manually. The BSA was not deleted.</font>\n"));

            }
        }
    }

    //    moveToTemp();

}


void Optimiser::renameBsa() //Renames the BSA.
{
    log->appendHtml(tr("\n\n\n<font color=Blue>Renaming BSA..."));

    QDirIterator it(modPath);

    while (it.hasNext())
    {
        if(it.next().contains(".bsa.bak"))
        {
            QFile bakBsa(it.filePath());
            QFile currentBsa(it.filePath().chopped(4));
            if(currentBsa.exists())
            {
                QCryptographicHash hash(QCryptographicHash::Sha1);

                bakBsa.open(QIODevice::ReadOnly);
                hash.addData(bakBsa.readAll());

                QByteArray bakBsaSig = hash.result();
                hash.reset();

                currentBsa.open(QIODevice::ReadOnly);
                hash.addData(currentBsa.readAll());

                QByteArray currentBsaSig = hash.result();

                if(currentBsaSig == bakBsaSig)
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
        else if(it.fileName().contains(".bsa"))
        {
            QFile::rename(it.filePath(), it.filePath() + ".bak");
        }
    }
}


void Optimiser::createBsa() //Once all the optimizations are done, create a new BSA
{
    log->appendHtml(tr("<font color=blue>\n\n\nCreating a new BSA...</font>"));


    if(dirSize(modPath + "/TempFolderWithRandomCharactersJustInCaseqzhdizqdhiqzdhi") > 2147483648)
    {
        log->appendHtml("<font color=Red>The BSA can't be created : more than 2gb of assets were detected</font>");
        restoreFromTemp(true, false);
        return;
    }

    bool hasSound=false;

    QDirIterator it(modPath);

    QProcess bsarch;
    QStringList bsarchArgs;

    QString espName = findEspName();
    QString tempPath = "/TempFolderWithRandomCharactersJustInCaseqzhdizqdhiqzdhi/";
    QDir modPathDir(modPath);

    QString bsaName = modPath + "/" + espName.chopped(4) + ".bsa";
    QString folderName = modPath + "/TempFolderWithRandomCharactersJustInCaseqzhdizqdhiqzdhi";

    bsarchArgs.clear();
    bsarchArgs << "pack" << folderName << bsaName << "-sse" << "-share";

    while (it.hasNext())
    {
        it.next();
        if(it.fileName().toLower() == "sound" || it.fileName().toLower() == "music")
            hasSound=true;
    }

    if (!hasSound) //Compressing BSA breaks sounds
    {
        bsarchArgs << "-z";
    }

    modPathDir.rename("TempFolderWithRandomCharactersJustInCaseqzhdizqdhiqzdhi/meshes/actors/character/animations", "meshes/actors/character/animations");

    log->appendPlainText(tr("Compressing...(this may take a long time, do not force close the program)\n"));

    bsarch.start("resources/bsarch.exe", bsarchArgs);

    bsarch.waitForFinished(-1);

    if(bsarch.readAllStandardOutput().contains("Done."))
    {
        log->appendHtml(tr("<font color=Blue> BSA successfully compressed.</font>\n"));
        modPathDir.setPath(modPath + tempPath);
        modPathDir.removeRecursively();
    }
}


void Optimiser::createTexturesBsa() // Create a BSA containing only textures
{
    log->appendHtml(tr("\n\n\n<font color=Blue>Creating a new textures BSA...</font>"));


    QProcess bsarch;
    QStringList bsarchArgs;
    QString espName = findEspName();
    QDir modPathDir;

    if(dirSize(modPath + "/TempFolderForTexturesThisTimeWithRandomCharsAlsodqzhduqz") > 2147483648)
    {
        log->appendHtml("<font color=Red>The BSA can't be created : more than 2gb of assets were detected</font>");
        restoreFromTemp(false, true);
        return;
    }

    if(modPathDir.exists(modPath + "/TempFolderForTexturesThisTimeWithRandomCharsAlsodqzhduqz/textures"))
    {
        log->appendPlainText(tr("Textures folder found. Compressing...(this may take a long time, do not force close the program)\n"));


        QString tBsaName= modPath + "/" + espName.chopped(4) + " - Textures.bsa";
        QString tFolder = modPath + "/TempFolderForTexturesThisTimeWithRandomCharsAlsodqzhduqz";

        bsarchArgs.clear();
        bsarchArgs << "pack" << tFolder << tBsaName << "-sse" << "-z" << "-share";

        bsarch.start("resources/bsarch.exe", bsarchArgs);
        bsarch.waitForFinished(-1);

        if(bsarch.readAllStandardOutput().contains("Done"))
        {
            modPathDir.setPath(modPath + "/TempFolderForTexturesThisTimeWithRandomCharsAlsodqzhduqz/");
            modPathDir.removeRecursively();
            log->appendHtml(tr("<font color=Blue> Textures BSA successfully compressed.</font>\n"));
        }
        else
        {
            log->appendHtml(tr("<font color=Red> An error occured during the Textures BSA compression."));
        }
    }
}



void Optimiser::bc7Conv(QDirIterator *it) //Compress the nmaps to BC7 if they are uncompressed.
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


void Optimiser::nifscanTextures() // Uses Nifscan with -fixdds option
{
    log->appendHtml(tr("\n\n\n<font color=Blue>Running Nifscan on the textures..."));


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


void Optimiser::tgaConv(QDirIterator* it) //Compress the nmaps to BC7 if they are uncompressed.
{
    log->appendHtml(tr("\n\n\n<font color=Blue>Converting TGA files..."));


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


void Optimiser::nifScan() //Nifscan all meshes and store it to a QStringList
{


    log->appendHtml(tr("\n\n\n<font color=Blue>Running NifScan..."));


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


void Optimiser::nifOpt(QDirIterator *it) // Optimise the meshes according to users option
{
    log->appendHtml(tr("\n\n\n<font color=Blue>Running NifOpt..."));
    log->appendPlainText(tr("Processing: ") + it->filePath());


    QProcess nifOpt;
    QStringList nifOptArgs;


    if(options.hardCrashingMeshes && crashingHeadparts.contains(it->filePath(), Qt::CaseInsensitive))
    {
        hardCrashingMeshes.removeAll(it->filePath());
        nifOptArgs.clear();
        nifOptArgs << it->filePath() << "-head" << "1" << "-bsTriShape" << "1";
    }

    else if (options.otherMeshes && otherHeadparts.contains(it->filePath(), Qt::CaseInsensitive))
    {
        nifOptArgs.clear();
        nifOptArgs << it->filePath() << "-head" << "1" << "-bsTriShape" << "1";
    }

    else if(options.otherMeshes && otherMeshes.contains(it->filePath()))
    {
        nifOptArgs.clear();
        nifOptArgs << it->filePath() << "-head" << "0" << "-bsTriShape" << "0";
    }

    else if(options.hardCrashingMeshes && hardCrashingMeshes.contains(it->filePath(), Qt::CaseInsensitive))
    {
        nifOptArgs.clear();
        nifOptArgs << it->filePath() << "-head" << "0" << "-bsTriShape" << "1";
    }

    nifOpt.start("resources/nifopt.exe", nifOptArgs);
    nifOpt.waitForFinished(-1);

}





void Optimiser::animOpt(QDirIterator *it) //Uses Bethesda Havok Tool to port animations
{
    log->appendHtml(tr("\n\n\n<font color=Blue>Processing animations..."));



    QProcess havokProcess;
    QStringList havokArgs;

    log->appendPlainText(tr("Current file: ") + it->filePath() + tr("\nProcessing...\n"));


    havokArgs.clear();
    havokArgs << "--platformamd64" << it->filePath() << it->filePath();
    havokProcess.start("resources/HavokBehaviorPostProcess.exe", havokArgs);

    havokProcess.waitForFinished(-1);



    if(havokProcess.readAllStandardOutput().isEmpty())
        log->appendHtml(tr("<font color=Blue>Animation successfully ported.</font>\n\n"));
    else
        log->appendHtml(tr("<font color=Grey> An error occured during the animation porting. Maybe it is already compatible with SSE ?</font>\n"));

}


QString Optimiser::findSkyrimDir() //Find Skyrim dir using Registry keys
{
    QSettings SkyrimReg(R"(HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Bethesda Softworks\Skyrim Special Edition)", QSettings::NativeFormat);
    QDir SkyrimDir = QDir::cleanPath(SkyrimReg.value("Installed Path").toString());
    return SkyrimDir.path();
}


QString Optimiser::findEspName() //Find esp name using an iterator
{
    QDirIterator it(modPath);
    QString espName;

    while (it.hasNext())
    {
        it.next();

        if(it.fileName().contains(".es"))
        {
            espName=it.fileName();
            log->appendPlainText(tr("Esp found.\n"));
            return espName;
        }
    }
    return "dummy_plugin.esp";
}


//Filesystem operations


void Optimiser::moveToTemp()
{
    QStringList assetsFolder;
    QDir modPathDir(modPath);
    QDirIterator it(modPath, QDirIterator::Subdirectories);

    QString tempPath = "/TempFolderWithRandomCharactersJustInCaseqzhdizqdhiqzdhi/";
    QString tempTexturesPath = "/TempFolderForTexturesThisTimeWithRandomCharsAlsodqzhduqz/";

    assetsFolder << "meshes" << "seq" << "scripts" << "dialogueviews" << "interface" << "source" << "lodsettings" << "strings" << "grass" << "sound" << "music";
    modPathDir.mkdir(modPath + tempPath);
    modPathDir.mkdir(modPath + tempTexturesPath);

    while (it.hasNext())
    {
        it.next();
        if(!it.filePath().contains(tempPath, Qt::CaseInsensitive) && assetsFolder.contains(it.fileName(), Qt::CaseInsensitive))
        {
            modPathDir.rename(it.fileName(), modPath + tempPath + it.fileName());
            log->appendHtml("<font color=Blue>Moving " + it.fileName() + " to a temporary folder\n</font>");
        }

        else if(!it.filePath().contains(tempPath, Qt::CaseInsensitive) && it.fileName().toLower() == "textures")
        {
            modPathDir.rename(it.fileName(), modPath + tempTexturesPath + it.fileName());
            log->appendHtml("<font color=Blue>Moving " + it.fileName() + " to a temporary folder\n</font>");
        }

        QCoreApplication::processEvents();
    }

    QDirIterator it2(modPath);

    while (it2.hasNext())
    {
        it2.next();

        if(assetsFolder.contains(it2.fileName(), Qt::CaseInsensitive))
        {

            if(modPathDir.exists(modPath + tempPath + it2.fileName()) && dirSize(modPath + tempPath + it2.fileName()) == dirSize(modPath + "/" + it2.fileName()))
            {
                log->appendHtml("<font color=Blue>Removing duplicated assets: </font>" + it2.fileName());
                modPathDir.setPath(modPath + "/" + it2.fileName());
                modPathDir.removeRecursively();
            }
            else if(modPathDir.exists(modPath + tempTexturesPath + it2.fileName()) && dirSize(modPath + tempTexturesPath + it2.fileName()) == dirSize(modPath + "/" + it2.fileName()))
            {
                log->appendHtml("<font color=Blue>Removing duplicated assets: </font>" + it2.fileName());
                modPathDir.setPath(modPath + "/" + it2.fileName());
                modPathDir.removeRecursively();
            }

            QCoreApplication::processEvents();
        }
    }
}


void Optimiser::restoreFromTemp(bool data, bool textures)
{
    QString tempPath = "/TempFolderWithRandomCharactersJustInCaseqzhdizqdhiqzdhi/";
    QString tempTexturesPath = "/TempFolderForTexturesThisTimeWithRandomCharsAlsodqzhduqz/";

    QDir modPathDir(modPath);

    if(data)
    {
        QDirIterator dataIt(modPath + tempPath);
        while(dataIt.hasNext())
        {
            dataIt.next();
            modPathDir.rename(dataIt.filePath(), dataIt.filePath().replace(tempPath, "/"));
        }
    }

    if(textures)
    {
        QDirIterator texturesIt(modPath + tempTexturesPath);
        while(texturesIt.hasNext())
        {
            texturesIt.next();
            modPathDir.rename(texturesIt.filePath(), texturesIt.filePath().replace(tempTexturesPath, "/"));
        }
    }

    deleteEmptyDirs(modPath);
}


void Optimiser::deleteEmptyDirs(const QString& path)
{
    QDirIterator it(path, QDirIterator::Subdirectories);
    QDir modPathDir(modPath);
    QFileInfo fileInfo;


    while(it.hasNext())
    {
        fileInfo.setFile(it.next());
        if(fileInfo.isDir() && dirSize(fileInfo.filePath()) == 0)
            modPathDir.rmdir(it.filePath());
    }
}


qint64 Optimiser::dirSize(const QString& path)
{
    qint64 size = 0;
    QDir dir(path);
    //calculate total size of current directories' files
    QDir::Filters fileFilters = QDir::Files|QDir::System|QDir::Hidden;
    for(const QString& filePath : dir.entryList(fileFilters)) {
        QFileInfo fi(dir, filePath);
        size+= fi.size();
    }
    //add size of child directories recursively
    QDir::Filters dirFilters = QDir::Dirs|QDir::NoDotAndDotDot|QDir::System|QDir::Hidden;
    for(const QString& childDirPath : dir.entryList(dirFilters))
        size+= dirSize(path + QDir::separator() + childDirPath);
    return size;
}


void Optimiser::saveSettings() //Saves settings to an ini file
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");

    settings.setValue("options.mode", options.mode);
    settings.setValue("DryRun", options.dryRun);
    settings.setValue("SelectedPath", options.userPath);

    settings.setValue("RecreateBSA", options.recreateBsa);
    settings.setValue("ExtractBSA", options.extractBsa);
    settings.setValue("PackExistingFiles", options.packExistingFiles);

    settings.setValue("HardCrashingMeshes", options.hardCrashingMeshes);
    settings.setValue("OtherMeshes", options.otherMeshes);

    settings.setValue("TGAConv", options.tgaConv);
    settings.setValue("BC7Conv", options.bc7Conv);
    settings.setValue("nifscanTextures", options.nifscanTextures);
}


void Optimiser::loadSettings() //Loads settings from the ini file
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);

    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");

    options.mode = settings.value("options.mode").toInt();
    options.dryRun = settings.value("DryRun").toBool();
    options.userPath = settings.value("SelectedPath").toString();

    options.recreateBsa = settings.value("RecreateBSA").toBool();
    options.extractBsa = settings.value("ExtractBSA").toBool();
    options.packExistingFiles = settings.value("PackExistingFiles").toBool();

    options.hardCrashingMeshes = settings.value("HardCrashingMeshes").toBool();
    options.otherMeshes = settings.value("OtherMeshes").toBool();

    options.tgaConv = settings.value("TGAConv").toBool();
    options.bc7Conv = settings.value("BC7Conv").toBool();
    options.nifscanTextures = settings.value("nifscanTextures").toBool();


}


void Optimiser::resetToDefaultSettings()
{
    options.mode = 0;
    options.userPath = "";

    options.recreateBsa = true;
    options.extractBsa = true;
    options.packExistingFiles = false;

    options.hardCrashingMeshes = true;
    options.otherMeshes = false;

    options.tgaConv = true;
    options.bc7Conv = true;
    options.nifscanTextures = true;

    options.animOptBool = true;

    options.dryRun = false;
}
