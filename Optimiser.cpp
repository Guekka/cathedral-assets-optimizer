#include "Optimiser.hpp"

const QString redColor("<font color=Red>");
const QString greenColor("<font color=Green>");
const QString greyColor("<font color=Grey>");
const QString orangeColor("<font color=Orange>");
const QString endColor("</font>\n");


Optimiser::Optimiser(QPlainTextEdit* textEdit, QProgressBar* bar) : log(textEdit), progressBar(bar), dummyPluginsCounter(0){}


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


    progressBar->setMaximum(modDirs.size()*(options.extractBsa + 1 + options.recreateBsa));

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

        if(options.recreateBsa)
        {
            createBsa();
            progressBar->setValue(progressBar->value() + 1);
        }

        QCoreApplication::processEvents();
    }

    //Deleting empty dirs
    system(QString("cd /d \"" + options.userPath + "\" && for /f \"delims=\" %d in ('dir /s /b /ad ^| sort /r') do rd \"%d\"").toStdString().c_str());

    progressBar->setValue(progressBar->maximum());
    log->appendHtml(greenColor + tr("Completed. Please read the above text to check if any errors occurred (displayed in red).") + endColor);


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
                log->appendHtml(greenColor + tr("BSA successfully extracted.") + endColor);
            }
            else
            {
                log->appendHtml(redColor + tr("An error occured during the extraction. Please extract it manually. The BSA was not deleted.") + endColor);
            }
        }
    }


}


void Optimiser::renameBsa() //Renames the BSA. TODO optimize execute time
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
    log->appendHtml(greenColor + tr("Creating a new BSA...") + endColor);


    QDir modPathDir(modPath);
    QStringList dirs;

    QProcess bsarch;

    modPathDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dirs = modPathDir.entryList();

    for (int i = 0; i < dirs.size(); ++i)
    {
        if(dirs.at(i).right(3) == "bsa")
        {
            bool hasSound=false;

            QTemporaryDir tempDir;
            QStringList bsarchArgs;

            QString folderName = modPath + "/" + dirs.at(i).right(dirs.size() - dirs.at(i).lastIndexOf("/)"));
            QString bsaName;

            QDir bsaDir(folderName);
            QStringList bsaDirs;
            bsaDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
            bsaDirs = bsaDir.entryList();

            //Detecting if BSA will contain sounds, since compressing sounds breaks them

            for (int j = 0; j < bsaDirs.size(); ++j)
            {
                if(bsaDirs.at(i).toLower() == "sound" || bsaDirs.at(i).toLower() == "music")
                        hasSound=true;
            }

            if(options.packExistingFiles)
            {
                if(folderName.isEmpty())
                {
                    folderName.replace(i, modPath + "/" + findEspName().chopped(4) + ".bsa");
                    if(folderName.isEmpty())
                    {
                        folderName = tempDir.path() + QString::number(i);
                    }
                }
                moveAssets(folderName + "/");
            }

            else if(folderName.isEmpty())
            {
                log->appendHtml(tr("No files to compress were found.") + "\n");
                return;
            }

            //Checking if it a textures BSA

            if(bsaDir.count() == 1 && bsaDirs.at(0).toLower() == "textures")
            {
                bsaName = folderName + " - Textures.bsa";
            }
            else
            {
                bsaName = folderName + ".bsa";
            }

            bsarchArgs << "pack" << folderName << bsaName << "-sse" << "-share";

            if (!hasSound) //Compressing BSA breaks sounds
            {
                bsarchArgs << "-z";
            }

            modPathDir.rename(folderName + "/meshes/actors/character/animations", "meshes/actors/character/animations"); //Moving animations because FNIS can't see them in BSA

            log->appendPlainText(tr("Compressing...(this may take a long time, do not force close the program)") + "\n");

            bsarch.start("resources/bsarch.exe", bsarchArgs);
            bsarch.waitForFinished(-1);

            qDebug() << "BSArch Args :" << bsarchArgs << "\nBSArch output :\n" << bsarch.readAllStandardOutput() << "\nBSA folder :" << folderName << "\nBsaName : " << bsaName;

            if(bsarch.readAllStandardOutput().contains("Done."))
            {
                if(QFile(bsaName).size() < 240000000)
                {
                    log->appendHtml(tr("BSA successfully compressed.") + "\n");
                    modPathDir.setPath(folderName);
                    modPathDir.removeRecursively();
                    modPathDir.rename(bsaName, folderName);
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
        log->appendHtml(greenColor + tr("Animation successfully ported.") + endColor);
    else
        log->appendHtml(greyColor + tr("An error occured during the animation porting. Maybe it is already compatible with SSE ?") + endColor);

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

        if(it.fileName().contains(QRegularExpression("\\.es[plm]")))
        {
            espName=it.fileName();
            log->appendPlainText(tr("Esp found:") + espName + "\n");
            return espName;
        }
    }
    ++dummyPluginsCounter;
    qDebug() << "dummy_plugin" + QString::number(dummyPluginsCounter) + ".esp";
    return "dummy_plugin" + QString::number(dummyPluginsCounter) + ".esp";
}


//Filesystem operations


void Optimiser::moveAssets(const QString& dest) //Move files with extensions present in assets list to specified folder
{
    QStringList assets;
    QDir modPathDir(modPath);
    QDirIterator it(modPath, QDirIterator::Subdirectories);
    QDirIterator firstIt(modPath);
    QFile looseAsset, bsaAsset;
    QString relativeFilename;

    assets << "nif" << "seq" << "pex" << "psc" << "lod" << "fuz" << "waw" << "xwm" << "swf" << "txt" << "hkx" << "dds" << "wav";

    while (firstIt.hasNext())
    {
        firstIt.next();
        if(firstIt.fileName().toLower() == "textures")
            modPathDir.rename(firstIt.fileName(), "TEXTURES"); //Making textures folder easily distinguishable
    }

    while (it.hasNext())
    {
        it.next();
        if(it.filePath().contains("TEXTURES") && !it.filePath().contains(".bsa"))
        {
            looseAsset.setFileName(it.filePath());
            relativeFilename = modPathDir.relativeFilePath(it.filePath());
            bsaAsset.setFileName(dest.chopped(4) + " - Textures.bsa/" + relativeFilename);

            if(bsaAsset.exists() && looseAsset.size() == bsaAsset.size())
            {
                QFile::remove(bsaAsset.fileName());
            }
            modPathDir.mkpath(bsaAsset.fileName().left(bsaAsset.fileName().lastIndexOf("/")));
            modPathDir.rename(looseAsset.fileName(), bsaAsset.fileName());
        }

        else if(!dest.contains(it.filePath()) && assets.contains(it.fileName().right(3)) && !it.filePath().contains(".bsa"))
        {
            looseAsset.setFileName(it.filePath());
            relativeFilename = modPathDir.relativeFilePath(it.filePath());
            bsaAsset.setFileName(dest + relativeFilename);

            if(bsaAsset.exists() && looseAsset.size() == bsaAsset.size())
            {
                QFile::remove(bsaAsset.fileName());
            }
            modPathDir.mkpath(bsaAsset.fileName().left(bsaAsset.fileName().lastIndexOf("/")));
            modPathDir.rename(looseAsset.fileName(), bsaAsset.fileName());
        }


        QCoreApplication::processEvents();
    }
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

    settings.setValue("AnimOpt", options.animOptBool);
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

    options.animOptBool = settings.value("AnimOpt").toBool();
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
