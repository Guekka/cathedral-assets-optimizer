#include "Optimiser.hpp"

const QString redColor("<font color=Red>");
const QString greenColor("<font color=Green>");
const QString greyColor("<font color=Grey>");
const QString orangeColor("<font color=Orange>");
const QString endColor("</font>");


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
        log->appendHtml(redColor + tr("Havok Tool not found. Are you sure the Creation Kit is installed ? You can also put HavokBehaviorPostProcess.exe in the resources folder") + endColor);
        return 1;
    }


    requirements << "bsarch.exe" << "NifScan.exe" << "nifopt.exe" << "texconv.exe" << "texdiag.exe" << "HavokBehaviorPostProcess.exe";

    for (int i = 0; i < requirements.size(); ++i)
    {
        QFile file("resources/" + requirements.at(i));
        if(!file.exists())
        {
            log->appendHtml(redColor + requirements.at(i) + tr(" not found. Cancelling.") + endColor);
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

    log->appendHtml(greenColor + tr("Beginning...") + endColor);

    for(int i=0; i < modDirs.size(); ++i)
    {
        modPath = QDir::cleanPath(options.userPath + "/" + modDirs.at(i));
        QDirIterator it(modPath, QDirIterator::Subdirectories);

        nifScan();

        log->appendHtml(orangeColor + tr("Current mod: ") + modPath + endColor);

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

        QCoreApplication::processEvents();
    }

    progressBar->setValue(progressBar->maximum());
    log->appendHtml(greenColor + tr("Completed. Please read the above text to check if any errors occurred (displayed in red).") + endColor + "\n");


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

        log->appendHtml(orangeColor + tr("Current mod: ") + modPath + endColor);

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
    }

    progressBar->setValue(progressBar->maximum());
    log->appendHtml(greenColor + tr("Completed.") + endColor);

}



void Optimiser::extractBsa() //Extracts the BSA
{
    log->appendHtml(greenColor + tr("Extracting BSA...") + endColor);


    QProcess bsarch;
    QStringList bsarchArgs;

    QDir modPathDir(modPath);

    QDirIterator it(modPath);

    renameBsa();

    while (it.hasNext())
    {
        if(it.next().contains(".bsa.bak"))
        {
            log->appendPlainText(tr("BSA found ! Extracting..."));

            QString bsaFolder = QDir::cleanPath(modPath + QDir::separator() + it.fileName().chopped(4));
            modPathDir.mkdir(bsaFolder);

            bsarchArgs.clear();
            bsarchArgs << "unpack" << it.filePath() << bsaFolder ;
            bsarch.start("resources/bsarch.exe", bsarchArgs);
            bsarch.waitForFinished(-1);

            if(bsarch.readAllStandardOutput().contains("Done."))
            {
                log->appendHtml(greenColor + tr("BSA successfully extracted.") + endColor + "\n");
            }
            else
            {
                log->appendHtml(redColor + tr("An error occured during the extraction. Please extract it manually. The BSA was not deleted.") + endColor + "\n");
            }
        }
    }


}


void Optimiser::renameBsa() //Renames the BSA.
{
    log->appendHtml(greenColor + tr("Renaming BSA...") + endColor + "\n");

    QDirIterator it(modPath);

    while (it.hasNext())
    {
        if(it.next().contains(".bsa.bak"))
        {
            QFile bakBsa(it.filePath());
            QFile currentBsa(it.filePath().chopped(4));
            if(currentBsa.exists() && !QFileInfo(currentBsa).isDir())
            {
                if(compareFiles(&bakBsa, &currentBsa))
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
    log->appendHtml(greenColor + tr("Creating a new BSA...") + endColor);

    bool hasSound=false;

    QDirIterator it(modPath, QDirIterator::Subdirectories);

    QProcess bsarch;
    QStringList bsarchArgs;

    QString espName = findEspName();
    QDir modPathDir(modPath);

    QString bsaName = modPath + "/" + espName.chopped(4) + ".bsa";
    QString folderName;


    while (it.hasNext())
    {
        it.next();
        if(it.fileName().toLower() == "sound" || it.fileName().toLower() == "music")
            hasSound=true;

        if(it.fileName().right(3) == "bsa")
            folderName = modPath + "/" + it.fileName();
    }


    if(options.packExistingFiles)
    {
        moveAssets(folderName + "/");
    }


    if(dirSize(modPath + "/" + folderName) > 2147483648)
    {
        log->appendHtml(redColor + tr("The BSA can't be created : more than 2gb of assets were detected") + endColor);
        return;
    }

    bsarchArgs << "pack" << folderName << bsaName << "-sse" << "-share";

    if (!hasSound) //Compressing BSA breaks sounds
    {
        bsarchArgs << "-z";
    }

    modPathDir.rename("TempFolderWithRandomCharactersJustInCaseqzhdizqdhiqzdhi/meshes/actors/character/animations", "meshes/actors/character/animations");

    log->appendPlainText(tr("Compressing...(this may take a long time, do not force close the program)") + "\n");

    bsarch.start("resources/bsarch.exe", bsarchArgs);

    bsarch.waitForFinished(-1);

    if(bsarch.readAllStandardOutput().contains("Done."))
    {
        log->appendHtml(greenColor + tr("BSA successfully compressed.") + endColor + "\n");
        modPathDir.setPath(modPath + tempPath);
        modPathDir.removeRecursively();
    }
}


void Optimiser::createTexturesBsa() // Create a BSA containing only textures
{
    log->appendHtml(greenColor + tr("Creating a new textures BSA...") + endColor);


    QProcess bsarch;
    QStringList bsarchArgs;
    QString espName = findEspName();
    QDir modPathDir;

    if(dirSize(modPath + "/TempFolderForTexturesThisTimeWithRandomCharsAlsodqzhduqz") > 2147483648)
    {
        log->appendHtml(redColor + tr("The BSA can't be created : more than 2gb of assets were detected") + endColor);
        return;
    }

    if(modPathDir.exists(modPath + "/TempFolderForTexturesThisTimeWithRandomCharsAlsodqzhduqz/textures"))
    {
        log->appendPlainText(tr("Textures folder found. Compressing...(this may take a long time, do not force close the program)") + "\n");


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
            log->appendHtml(greenColor + tr(" Textures BSA successfully compressed.") + endColor + "\n");
        }
        else
        {
            log->appendHtml(redColor + tr(" An error occured during the Textures BSA compression.") + endColor);
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


void Optimiser::tgaConv(QDirIterator* it) //Compress the nmaps to BC7 if they are uncompressed.
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


void Optimiser::nifScan() //Nifscan all meshes and store it to a QStringList
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


void Optimiser::nifOpt(QDirIterator *it) // Optimise the meshes according to users option
{
    log->appendHtml(greenColor +  tr("Running NifOpt...") + endColor);
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
    log->appendHtml(greenColor + tr("Processing animations...") + endColor);



    QProcess havokProcess;
    QStringList havokArgs;

    log->appendPlainText(tr("Current file: ") + it->filePath() + tr("\nProcessing...\n"));


    havokArgs.clear();
    havokArgs << "--platformamd64" << it->filePath() << it->filePath();
    havokProcess.start("resources/HavokBehaviorPostProcess.exe", havokArgs);

    havokProcess.waitForFinished(-1);



    if(havokProcess.readAllStandardOutput().isEmpty())
        log->appendHtml(greenColor + tr("Animation successfully ported.") + endColor + "\n");
    else
        log->appendHtml(greyColor + tr("An error occured during the animation porting. Maybe it is already compatible with SSE ?") + endColor + "\n");

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


void Optimiser::moveAssets(const QString& dest) //Move files with extensions present in assets list to specified folder
{
    QStringList assets;
    QDir modPathDir(modPath);
    QDirIterator it(modPath, QDirIterator::Subdirectories);
    QFile looseAsset, bsaAsset;

    assets << "nif" << "seq" << "pex" << "dds" << "psc" << "lod" << "fuz" << "waw" << "xwm" << "swf" << "txt" << "hkx";

    while (it.hasNext())
    {

        if(!dest.contains(it.filePath()) && assets.contains(it.fileName().right(3)))
        {
            looseAsset.setFileName(it.next());
            QString file(modPathDir.relativeFilePath(it.filePath()));
            bsaAsset.setFileName(dest + file);

            if(bsaAsset.exists() && compareFiles(&looseAsset, &bsaAsset))
            {
                QFile::remove(dest + file);
            }
            modPathDir.rename(file, dest + file);
            log->appendHtml(greenColor + tr("Moving ") + endColor + it.fileName() + greenColor + tr(" to a temporary folder") + endColor + "\n");
        }
        QCoreApplication::processEvents();
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
        if(fileInfo.isDir() && dirSize(fileInfo.filePath()) == 0 && !it.fileName().contains("_separator"))
            modPathDir.rmdir(it.filePath());
    }

    //Doing it twice because sometimes empty directories are still there

    QDirIterator it2(path, QDirIterator::Subdirectories);

    while(it2.hasNext())
    {
        fileInfo.setFile(it2.next());
        if(fileInfo.isDir() && dirSize(fileInfo.filePath()) == 0 && !it2.fileName().contains("_separator"))
            modPathDir.rmdir(it2.filePath());
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


bool Optimiser::compareFiles(QFile* file1, QFile* file2)
{
    QCryptographicHash hash(QCryptographicHash::Sha1);

    file1->open(QIODevice::ReadOnly);
    hash.addData(file1->readAll());

    QByteArray file1Sig = hash.result();
    hash.reset();

    file2->open(QIODevice::ReadOnly);
    hash.addData(file2->readAll());

    QByteArray file2Sig = hash.result();

    if(file1Sig == file2Sig)
    {
        return true;
    }

    return false;
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
