#include "Optimiser.hpp"


Optimiser::Optimiser(QPlainTextEdit* textEdit, QProgressBar* bar) : log(textEdit), progressBar(bar){}


int Optimiser::mainProcess() // Process the userPath according to all user options
{
    saveSettings();
    progressBar->reset();

    log->clear();
    log->appendHtml(QPlainTextEdit::tr("<font color=Blue>Beginning...</font>"));

    QStringList modDirs;
    QStringList requirements;

    requirements << "bsarch.exe" << "NifScan.exe" << "nifopt.exe" << "texconv.exe" << "texdiag.exe";

    for (int i = 0; i < requirements.size(); ++i)
    {
        QFile file("resources/" + requirements.at(i));
        if(!file.exists())
        {
            log->appendHtml("<font color=Red>" + requirements.at(i) + QPlainTextEdit::tr(" not found. Cancelling."));
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
        modDirs << "";
    }


    progressBar->setMaximum(modDirs.size()*6);


    for(int i=0; i < modDirs.size(); ++i)
    {
        modPath = options.userPath + "/" + modDirs.at(i);
        QDirIterator it(modPath, QDirIterator::Subdirectories);

        log->appendHtml("<font color=Orange>" + QPlainTextEdit::tr("Current mod: ") + modPath + "</font>");

        if(options.extractBsa)
        {
            extractBsa();
            progressBar->setValue(progressBar->value() + 1);
        }

        if(options.renameBsa)
        {
            renameBsa();
            progressBar->setValue(progressBar->value() + 1);
        }

        if(options.nifscanTextures)
        {
            nifscanTextures();
            progressBar->setValue(progressBar->value() + 1);
        }

        while(it.hasNext())
        {
            it.next();
            QCoreApplication::processEvents();

            if((options.hardCrashingMeshes || options.otherMeshes) && it.fileName().contains(".nif", Qt::CaseInsensitive))
                nifOpt(&it);

            if((options.bc7Conv) && it.fileName().contains("_n.dds", Qt::CaseInsensitive))
                bc7Conv(&it);

            if((options.tgaConv) && it.fileName().contains(".tga", Qt::CaseInsensitive))
                tgaConv(&it);

            if(animOptBool && it.fileName().contains(".hkx", Qt::CaseInsensitive))
                animOpt(&it);

        }

        progressBar->setValue(progressBar->value() + 1);

        if(options.createBsa)
        {
            createBsa();
            progressBar->setValue(progressBar->value() + 1);
            createTexturesBsa();
            progressBar->setValue(progressBar->value() + 1);
        }

        QCoreApplication::processEvents();
    }

    progressBar->setValue(progressBar->maximum());
    log->appendHtml(QPlainTextEdit::tr("<font color=blue>Completed. Please check the log to check if any errors occured(in red) </font>\n"));
    log->repaint();

    return 0;
}


bool Optimiser::extractBsa() //Extracts the BSA
{
    log->appendHtml(QPlainTextEdit::tr("\n\n\n<font color=Blue>Extracting BSA..."));
    log->repaint();


    QProcess bsarch;
    QStringList bsarchArgs;

    QDirIterator it(modPath);

    while (it.hasNext())
    {
        if(it.next().contains(".bsa") && !it.fileName().contains(".bak"))
        {
            log->appendPlainText(QPlainTextEdit::tr("BSA found ! Extracting..."));
            log->repaint();

            bsarchArgs.clear();
            bsarchArgs << "unpack" << it.filePath() << it.path() ;
            bsarch.start("resources/bsarch.exe", bsarchArgs);
            bsarch.waitForFinished(-1);


            if(bsarch.readAllStandardOutput().contains("Done."))
            {
                log->appendHtml(QPlainTextEdit::tr("<font color=Blue>BSA successfully extracted.</font>\n"));
                log->repaint();
            }
            else{
                log->appendHtml(QPlainTextEdit::tr("<font color=Red>An error occured during the extraction. Please extract it manually. The BSA was not deleted.</font>\n"));
                log->repaint();
            }
        }
    }
    return true;
}


bool Optimiser::renameBsa() //Renames the BSA.
{
    log->appendHtml(QPlainTextEdit::tr("\n\n\n<font color=Blue>Renaming BSA..."));
    log->repaint();

    QStringList bsarchArgs;
    QDirIterator it(modPath);

    while (it.hasNext())
    {
        if(it.next().contains(".bsa") && !it.fileName().contains(".bak"))
        {
            QFile::rename(it.filePath(), it.filePath() + ".bak");
            log->appendPlainText(QPlainTextEdit::tr("BSA successfully renamed.\n"));
            log->repaint();
        }
    }
    return true;
}


bool Optimiser::bc7Conv(QDirIterator *it) //Compress the nmaps to BC7 if they are uncompressed.
{
    log->appendHtml(QPlainTextEdit::tr("\n\n\n<font color=Blue>Converting normal maps to BC7..."));
    log->repaint();

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

        log->appendPlainText(QPlainTextEdit::tr("\nUncompressed normal map found:\n") + it->fileName() + QPlainTextEdit::tr("\nCompressing..."));
        texconv.start("resources/texconv.exe",  texconvArg);
        texconv.waitForFinished(-1);

        log->repaint();

        return true;
    }
    return false;
}


bool Optimiser::nifscanTextures() // Uses Nifscan with -fixdds option
{
    log->appendHtml(QPlainTextEdit::tr("\n\n\n<font color=Blue>Running Nifscan on the textures..."));
    log->repaint();

    QFile nifScan_file("resources/NifScan.exe");
    QProcess nifScan;
    QStringList NifscanArgs;

    NifscanArgs << "-fixdds";

    QFile::copy("resources/NifScan.exe", modPath + "/NifScan.exe");
    nifScan.setWorkingDirectory(modPath);

    nifScan.start(modPath + "/NifScan.exe", NifscanArgs);
    nifScan.waitForFinished(-1);

    QFile::remove(modPath + "/NifScan.exe");

    return true;
}


bool Optimiser::tgaConv(QDirIterator* it) //Compress the nmaps to BC7 if they are uncompressed.
{
    log->appendHtml(QPlainTextEdit::tr("\n\n\n<font color=Blue>Converting TGA files..."));
    log->repaint();

    QProcess texconv;
    QStringList texconvArg;

    log->appendPlainText(QPlainTextEdit::tr("\nTGA file found: \n") + it->fileName() + QPlainTextEdit::tr("\nCompressing..."));
    log->repaint();

    texconvArg.clear();
    texconvArg << "-nologo" << "-m" << "0" << "-y" << "-pow2" << "-if" << "FANT" << "-f" << "R8G8B8A8_UNORM" << it->filePath();
    texconv.start("resources/texconv.exe",  texconvArg);
    texconv.waitForFinished(-1);

    QFile tga(it->filePath());
    tga.remove();

    return true;
}


bool Optimiser::nifScan() //Nifscan all meshes and store it to a QStringList
{
    log->appendHtml(QPlainTextEdit::tr("\n\n\n<font color=Blue>Running NifScan..."));
    log->repaint();

    QString readLine;
    QString currentFile;

    QFile nifScan_file("resources/NifScan.exe");

    QProcess nifScan;

    meshes.clear();
    hardCrashingMeshes.clear();

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
            meshes << currentFile;
        }

        else if(readLine.contains("unsupported", Qt::CaseInsensitive) || readLine.contains("not supported", Qt::CaseInsensitive))
        {
            hardCrashingMeshes << currentFile;
        }
    }

    meshes.removeDuplicates();
    hardCrashingMeshes.removeDuplicates();

    QFile::remove(modPath + "/NifScan.exe");

    return true;
}


bool Optimiser::nifOpt(QDirIterator *it) // Optimise the meshes according to users option
{
    log->appendHtml(QPlainTextEdit::tr("\n\n\n<font color=Blue>Running NifOpt..."));
    log->appendPlainText(QPlainTextEdit::tr("Processing: ") + it->filePath());
    log->repaint();

    QProcess nifOpt;
    QStringList nifOptArgs;

    if(!options.hardCrashingMeshes && !hardCrashingMeshes.contains(it->filePath(), Qt::CaseInsensitive) && it->filePath().contains("facegendata", Qt::CaseInsensitive))
    {
        nifOptArgs.clear();
        nifOptArgs << it->filePath() << "-head" << "1" << "-bsTriShape" << "1";
    }

    else if(!options.hardCrashingMeshes && !hardCrashingMeshes.contains(it->filePath(), Qt::CaseInsensitive))
    {
        nifOptArgs.clear();
        nifOptArgs << it->filePath() << "-head" << "0" << "-bsTriShape" << "0";
    }

    else if(hardCrashingMeshes.contains(it->filePath(), Qt::CaseInsensitive))
    {
        nifOptArgs.clear();
        nifOptArgs << it->filePath() << "-head" << "0" << "-bsTriShape" << "1";
    }

    nifOpt.start("resources/nifopt.exe", nifOptArgs);
    nifOpt.waitForFinished(-1);

    return true;
}


bool Optimiser::createBsa() //Once all the optimizations are done, create a new BSA
{    
    log->appendHtml(QPlainTextEdit::tr("<font color=blue>\n\n\nCreating a new BSA...</font>"));
    log->repaint();

    bool hasSound=false;

    QDirIterator it(modPath);

    QProcess bsarch;
    QStringList bsarchArgs;

    QString espName = findEspName();
    QDir modPathDir(modPath);

    QStringList assetsFolder;
    assetsFolder << "meshes" << "seq" << "scripts" << "dialogueviews" << "interface" << "source" << "lodsettings";

    modPathDir.mkdir("data");

    while (it.hasNext())
    {
        it.next();

        if(it.fileName().toLower() == "sound")
        {
            hasSound=true;
            modPathDir.rename(it.filePath(), "data/" + it.fileName());
        }
        if(assetsFolder.contains(it.fileName().toLower()))
        {
            modPathDir.rename(it.filePath(), "data/" + it.fileName());
        }
    }

    QString bsaName = modPath + "/data/" + espName.chopped(4) + ".bsa";
    QString folderName = modPath + "/data";
    bsarchArgs.clear();
    bsarchArgs << "pack" << folderName << bsaName << "-sse" << "-share";

    log->appendPlainText(QPlainTextEdit::tr("Compressing...(this may take a long time, do not force close the program)\n"));

    if (!hasSound) //Compressing BSA breaks sounds
    {
        bsarchArgs << "-z";
    }

    if(QFileInfo(modPath + "/data").size() > 2000000000)
        return false;

    modPathDir.rmdir(modPath + "/data/");

    bsarch.start("resources/bsarch.exe", bsarchArgs);
    bsarch.waitForFinished(-1);


    if(bsarch.readAllStandardOutput().contains("Done."))
    {
        log->appendHtml(QPlainTextEdit::tr("<font color=Blue> BSA successfully compressed.</font>\n"));
        modPathDir.rename(bsaName, modPath + "/" + espName.chopped(4) + ".bsa");
        modPathDir.setPath(modPath + "/data");
        modPathDir.removeRecursively();
        return true;
    }
    return false;
}


bool Optimiser::createTexturesBsa() // Create a BSA containing only textures
{
    log->appendHtml(QPlainTextEdit::tr("\n\n\n<font color=Blue>Creating a new textures BSA...</font>"));
    log->repaint();

    QDirIterator it(modPath);
    QProcess bsarch;
    QStringList bsarchArgs;
    QString espName = findEspName();
    QDir modPathDir(modPath);

    while (it.hasNext())
    {
        it.next();

        if(it.fileName().toLower() == "textures")
        {
            log->appendPlainText(QPlainTextEdit::tr("Textures folder found. Compressing...(this may take a long time, do not force close the program)\n"));
            log->repaint();

            QString tBsaName= it.path() + "/" + espName.chopped(4) + " - Textures.bsa";
            QString tFolder = it.path() + "/temp_t";

            modPathDir.mkdir("temp_t");

            modPathDir.rename(it.filePath(), "temp_t/" + it.fileName());

            bsarchArgs.clear();
            bsarchArgs << "pack" << tFolder << tBsaName << "-sse" << "-z" << "-share";

            if(QFileInfo(modPath + "/temp_t").size() > 2000000000)
                return false;

            bsarch.start("resources/bsarch.exe", bsarchArgs);
            bsarch.waitForFinished(-1);


            if(bsarch.readAllStandardOutput().contains("Done"))
            {
                modPathDir.setPath(modPath + "/temp_t/");
                modPathDir.removeRecursively();
                log->appendHtml(QPlainTextEdit::tr("<font color=Blue> Textures BSA successfully compressed.</font>\n"));
            }
            else
            {
                log->appendHtml(QPlainTextEdit::tr("<font color=Red> An error occured during the Textures BSA compression."));
            }
            return true;
        }
    }
    return false;
}


bool Optimiser::animOpt(QDirIterator *it) //Uses Bethesda Havok Tool to port animations
{
    log->appendHtml(QPlainTextEdit::tr("\n\n\n<font color=Blue>Processing animations..."));
    log->repaint();

    QFile havokFile(findSkyrimDir() + "/Tools/HavokBehaviorPostProcess/HavokBehaviorPostProcess.exe");

    if(havokFile.exists())
    {
        log->appendPlainText(QPlainTextEdit::tr("Havok tool found.\n"));

        QProcess havokProcess;
        QStringList havokArgs;

        log->appendPlainText(QPlainTextEdit::tr("Current file: ") + it->filePath() + QPlainTextEdit::tr("\nProcessing...\n"));
        log->repaint();

        havokArgs.clear();
        havokArgs << "--platformamd64" << it->filePath() << it->filePath();
        havokProcess.start(havokFile.fileName(), havokArgs);
        havokProcess.waitForFinished(-1);

        if(havokProcess.readAllStandardOutput().isEmpty())
            log->appendHtml(QPlainTextEdit::tr("<font color=Blue>Animation successfully ported.</font>\n\n"));
        else
            log->appendHtml(QPlainTextEdit::tr("<font color=Brown> An error occured during the animation porting. Maybe it is already compatible with SSE ?</font>\n"));
    }
    else
    {
        log->appendHtml(QPlainTextEdit::tr("<font color=Red>Havok Tool not found. Are you sure the Creation Kit is installed ?</font>\n"));
        return false;
    }
    return true;
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
            log->appendPlainText(QPlainTextEdit::tr("Esp found.\n"));
            return espName;
        }
    }
    return "dummy_plugin.esp";
}


void Optimiser::saveSettings() //Saves settings to an ini file
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);

    if(options.createBsa && options.extractBsa && options.renameBsa)
    {
        bsaOptBool = true;
    }
    else
    {
        bsaOptBool = false;
    }

    if(options.hardCrashingMeshes && options.otherMeshes)
    {
        nifOptBool = true;
    }
    else
    {
        nifOptBool = false;
    }

    if(options.tgaConv && options.bc7Conv && options.nifscanTextures)
    {
        textOptBool = true;
    }
    else
    {
        textOptBool = false;
    }

    settings.setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");
    settings.clear();

    settings.setValue("options.mode", options.mode);
    settings.setValue("SelectedPath", options.userPath);

    settings.setValue("OptimiseBSA", bsaOptBool);
    settings.setValue("CreateBSA", options.createBsa);
    settings.setValue("ExtractBSA", options.extractBsa);
    settings.setValue("RenameBsa", options.renameBsa);

    settings.setValue("OptimiseMeshes", nifOptBool);
    settings.setValue("HardCrashingMeshes", options.hardCrashingMeshes);
    settings.setValue("OtherMeshes", options.otherMeshes);

    settings.setValue("OptimiseTextures", textOptBool);
    settings.setValue("TGAConv", options.tgaConv);
    settings.setValue("BC7Conv", options.bc7Conv);
    settings.setValue("nifscanTextures", options.nifscanTextures);

    settings.setValue("OptimiseAnimations", animOptBool);



}


void Optimiser::loadSettings() //Loads settings from the ini file
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);

    settings.setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");

    options.mode = settings.value("options.mode").toInt();
    options.userPath = settings.value("SelectedPath").toString();

    bsaOptBool = settings.value("OptimiseBSA").toBool();
    options.createBsa = settings.value("CreateBSA").toBool();
    options.extractBsa = settings.value("ExtractBSA").toBool();
    options.renameBsa = settings.value("RenameBsa").toBool();

    nifOptBool = settings.value("OptimiseMeshes").toBool();
    options.hardCrashingMeshes = settings.value("HardCrashingMeshes").toBool();
    options.otherMeshes = settings.value("OtherMeshes").toBool();

    textOptBool = settings.value("OptimiseTextures").toBool();
    options.tgaConv = settings.value("TGAConv").toBool();
    options.bc7Conv = settings.value("BC7Conv").toBool();
    options.nifscanTextures = settings.value("nifscanTextures").toBool();

    animOptBool = settings.value("OptimiseAnimations").toBool();

    if(!options.renameBsa)
        options.createBsa = false;
}


bool Optimiser::getBsaOptBool()
{
    return bsaOptBool;
}


bool Optimiser::getTextOptBool()
{
    return  textOptBool;
}


bool Optimiser::getNifOptBool()
{
    return nifOptBool;
}


bool Optimiser::getAnimOptBool()
{
    return  animOptBool;
}


void Optimiser::setBsaOptBool(bool state)
{
    bsaOptBool = state;

    options.extractBsa = state;
    options.createBsa = state;
    options.renameBsa = state;

    saveSettings();
}


void Optimiser::setTextOptBool(bool state)
{
    textOptBool = state;

    options.tgaConv = state;
    options.bc7Conv = state;
    options.nifscanTextures = state;

    saveSettings();
}


void Optimiser::setNifOptBool(bool state)
{
    nifOptBool = state;

    options.hardCrashingMeshes = state;
    options.otherMeshes = state;

    saveSettings();
}


void Optimiser::setAnimOptBool(bool state)
{
    animOptBool = state;

    saveSettings();
}


void Optimiser::dryRun()
{
    saveSettings();
    progressBar->reset();

    log->clear();
    log->appendHtml(QPlainTextEdit::tr("<font color=Blue>Beginning...</font>"));

    QStringList modDirs;
    QStringList requirements;

    requirements << "bsarch.exe" << "NifScan.exe" << "nifopt.exe" << "texconv.exe" << "texdiag.exe";

    for (int i = 0; i < requirements.size(); ++i)
    {
        QFile file("resources/" + requirements.at(i));
        if(!file.exists())
        {
            log->appendHtml("<font color=Red>" + requirements.at(i) + QPlainTextEdit::tr(" not found. Cancelling."));
            return ;
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
        modDirs << "";
    }


    progressBar->setMaximum(modDirs.size()*6);


    for(int i=0; i < modDirs.size(); ++i)
    {
        modPath = options.userPath + "/" + modDirs.at(i);
        QDirIterator it(modPath, QDirIterator::Subdirectories);

        log->appendHtml("<font color=Orange>" + QPlainTextEdit::tr("Current mod: ") + modPath + "</font>");

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
                if(!options.hardCrashingMeshes && !hardCrashingMeshes.contains(it.filePath(), Qt::CaseInsensitive) && it.filePath().contains("facegendata", Qt::CaseInsensitive))
                {
                    log->appendPlainText(it.filePath() + QPlainTextEdit::tr(" would be optimised with headparts mesh option.\n"));
                }

                else if(!options.hardCrashingMeshes && !hardCrashingMeshes.contains(it.filePath(), Qt::CaseInsensitive))
                {
                    log->appendPlainText(it.filePath() + QPlainTextEdit::tr(" would be optimised with light mesh option.\n"));
                }

                else if(hardCrashingMeshes.contains(it.filePath(), Qt::CaseInsensitive))
                {
                    log->appendPlainText(it.filePath() + QPlainTextEdit::tr(" would be optimised with full mesh option.\n"));
                }

            }
            if((options.bc7Conv) && it.fileName().contains("_n.dds", Qt::CaseInsensitive))
            {
                QProcess texDiag;
                QStringList texdiagArg;

                texdiagArg << "info" << it.filePath();

                texDiag.start("resources/texdiag.exe", texdiagArg);
                texDiag.waitForFinished(-1);

                if(texDiag.readAllStandardOutput().contains("compressed = no"))
                {
                    log->appendPlainText(it.filePath() + QPlainTextEdit::tr(" would be compressed to BC7\n"));
                }
            }

            if((options.tgaConv) && it.fileName().contains(".tga", Qt::CaseInsensitive))
            {
                log->appendPlainText(it.filePath() + QPlainTextEdit::tr(" would be converted to DDS"));
            }
        }

        progressBar->setValue(progressBar->value() + 1);

        QCoreApplication::processEvents();
    }

    progressBar->setValue(progressBar->maximum());
    log->appendHtml(QPlainTextEdit::tr("<font color=blue>Completed.</font>\n"));
    log->repaint();

    return;
}




