#include "Optimiser.hpp"


Optimiser::Optimiser(QString mod, QPlainTextEdit* textEdit, QProgressBar* bar) : userPath(std::move(mod)), log(textEdit), progressBar(bar){}


int Optimiser::mainProcess()
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

    if(mode == 1)
    {
        QDir dir(userPath);
        dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
        modDirs = dir.entryList();
    }

    if(mode == 0)
    {
        modDirs << "";
    }

    progressBar->setMaximum(modDirs.size()*6);

    for(int i=0; i < modDirs.size(); ++i)
    {
        modPath = userPath + "/" + modDirs.at(i);

        log->appendHtml("<font color=Orange>" + QPlainTextEdit::tr("Current mod: ") + modPath+ + "</font>");

        if(extractBsaBool)
        {
            extractBsa();
            progressBar->setValue(progressBar->value() + 1);
            progressBar->repaint();
        }

        if(renameBsaBool)
        {
            renameBsa();
        }

        if(nifOptBool)
        {
            nifOpt();
            progressBar->setValue(progressBar->value() + 1);
            progressBar->repaint();
        }

        if(textOptBool)
        {
            textOpt();
            progressBar->setValue(progressBar->value() + 1);
            progressBar->repaint();
        }

        if(animOptBool)
            animOpt();
        progressBar->setValue(progressBar->value() + 1);
        progressBar->repaint();

        if(createBsaBool)
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
    log->appendHtml(QPlainTextEdit::tr("\n\n\n<font color=Blue>Processing BSA..."));
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


bool Optimiser::renameBsa() //Deletes the BSA.
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


bool Optimiser::textOpt() //Compress the nmaps to BC7 if they are uncompressed.
{

    log->appendHtml(QPlainTextEdit::tr("\n\n\n<font color=Blue>Processing textures..."));
    log->repaint();


    QFile nifScan_file("resources/NifScan.exe");
    QProcess nifScan;
    QStringList NifscanArgs;

    QProcess texDiag;
    QStringList texdiagArg;

    QProcess texconv;
    QStringList texconvArg;

    QDirIterator it(modPath, QDirIterator::Subdirectories);

    NifscanArgs << "-fixdds";

    QFile::copy("resources/NifScan.exe", modPath + "/NifScan.exe");
    nifScan.setWorkingDirectory(modPath);

    nifScan.start(modPath + "/NifScan.exe", NifscanArgs);
    nifScan.waitForFinished(-1);

    QFile::remove(modPath + "/NifScan.exe");

    while (it.hasNext())
    {
        it.next();

        if(it.fileName().contains("_n.dds"))
        {
            log->appendPlainText(QPlainTextEdit::tr("\nCurrently scanning :\n") + it.fileName());
            log->repaint();

            texdiagArg << "info" << it.filePath();

            texDiag.start("resources/texdiag.exe", texdiagArg);
            texDiag.waitForFinished(-1);


            if(texDiag.readAllStandardOutput().contains("compressed = no"))
            {
                texconvArg.clear();
                texconvArg << "-nologo" << "-y" << "-m" << "0" << "-pow2" << "-if" << "FANT" << "-f" << "BC7_UNORM" << it.filePath();

                texconv.start("resources/texconv.exe",  texconvArg);
                texconv.waitForFinished(-1);
                log->appendPlainText(QPlainTextEdit::tr("\nUncompressed normal map found:\n") + it.fileName() + QPlainTextEdit::tr("\nCompressing..."));
                log->repaint();
            }
            it.next();
        }
        else if(it.fileName().contains(".tga"))
        {

            log->appendPlainText(QPlainTextEdit::tr("\nTGA file found: \n") + it.fileName() + QPlainTextEdit::tr("\nCompressing..."));
            log->repaint();

            texconvArg.clear();
            texconvArg << "-nologo" << "-m" << "0" << "-y" << "-pow2" << "-if" << "FANT" << "-f" << "R8G8B8A8_UNORM" << it.filePath();
            texconv.start("resources/texconv.exe",  texconvArg);
            texconv.waitForFinished(-1);

            QFile tga(it.filePath());
            tga.remove();

            it.next();
        }
    }
    return true;
}


bool Optimiser::nifOpt() // Optimise the meshes if Nifscan report them.
{
    log->appendHtml(QPlainTextEdit::tr("\n\n\n<font color=Blue>Processing meshes..."));
    log->repaint();


    bool meshProcessed=false; //To avoid processing twice the same mesh

    QString readLine;
    QString currentFile;

    QFile nifScan_file("resources/NifScan.exe");

    QProcess nifScan;
    QProcess nifOpt;


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
            currentFile = QDir::cleanPath(modPath + "/" + readLine.simplified());
            log->appendPlainText(QPlainTextEdit::tr("\nProcessing : ") + currentFile);
            meshProcessed=false;
        }
        else if((readLine.contains("unsupported") || readLine.contains("not supported")) && !meshProcessed)
        {
            meshProcessed=true;
            nifOpt.start("resources/nifopt.exe \"" + currentFile + "\" -head 0");
            nifOpt.waitForFinished(-1);

            //   log->appendHtml(QPlainTextEdit::tr("<font color=\"Blue\"> Mesh ported to SSE\n"));
            //   log->appendHtml(QPlainTextEdit::tr("<font color=\"Red\"> Error while porting the mesh\n"));
        }
        if(readLine.contains("facegendata", Qt::CaseInsensitive))
        {
            nifOpt.start("resources/nifopt.exe \"" + currentFile + "\" -head 1");
            nifOpt.waitForFinished(-1);
        }
    }
    QFile::remove(modPath + "/NifScan.exe");

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


bool Optimiser::animOpt() //Uses Bethesda Havok Tool to port animations
{
    log->appendHtml(QPlainTextEdit::tr("\n\n\n<font color=Blue>Processing animations..."));
    log->repaint();

    QFile havokFile(findSkyrimDir() + "/Tools/HavokBehaviorPostProcess/HavokBehaviorPostProcess.exe");

    if(havokFile.exists())
    {
        log->appendPlainText(QPlainTextEdit::tr("Havok tool found.\n"));

        QProcess havokProcess;
        QStringList havokArgs;

        QDirIterator it(modPath, QDirIterator::Subdirectories);

        while (it.hasNext())
        {
            if(it.next().toLower().contains(".hkx"))
            {
                log->appendPlainText(QPlainTextEdit::tr("Current file: ") + it.filePath() + QPlainTextEdit::tr("\nProcessing...\n"));
                log->repaint();

                havokArgs.clear();
                havokArgs << "--platformamd64" << it.filePath() << it.filePath();
                havokProcess.start(havokFile.fileName(), havokArgs);
                havokProcess.waitForFinished(-1);

                if(havokProcess.readAllStandardOutput().isEmpty())
                    log->appendHtml(QPlainTextEdit::tr("<font color=Blue>Animation successfully ported.</font>\n\n"));
                else
                    log->appendHtml(QPlainTextEdit::tr("<font color=Red> An error occured during the animation porting. Maybe it is already compatible with SSE ?</font>\n"));
            }
        }
    }
    else
    {
        log->appendHtml(QPlainTextEdit::tr("<font color=Red>Havok Tool not found. Are you sure the Creation Kit is installed ?</font>\n"));
        return false;
    }
    return true;
}


bool Optimiser::setUserPath(const QString& path)
{
    QFileInfo modDir(path);
    if(modDir.isDir())
    {
        userPath = path;
        return true;
    }

    return false;

}


QString Optimiser::getUserPath() const
{
    return userPath;
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


void Optimiser::setMode(int index)
{
    mode = index;
    saveSettings();
}


void Optimiser::setExtractBsaBool(bool state)
{
    extractBsaBool = !state;
    saveSettings();
}


void Optimiser::setTextOptBool(bool state)
{
    textOptBool = !state;
    saveSettings();
}


void Optimiser::setNifOptBool(bool state)
{
    nifOptBool = !state;
    saveSettings();
}


void Optimiser::setCreateBsaBool(bool state)
{
    createBsaBool = !state;
    saveSettings();
}


void Optimiser::setRenameBsaBool(bool state)
{
    renameBsaBool = !state;
    saveSettings();
}


void Optimiser::setAnimOptBool(bool state)
{
    animOptBool = !state;
    saveSettings();
}


void Optimiser::setBsaOptBool(bool state)
{
    bsaOptBool = !state;
    renameBsaBool = !state;
    extractBsaBool = !state;
    createBsaBool = !state;
    saveSettings();
}


bool Optimiser::getBsaOptBool()
{
    return bsaOptBool;
}


bool Optimiser::getExtractBsaBool()
{
    return extractBsaBool;
}


bool Optimiser::getTextOptBool()
{
    return textOptBool;
}


bool Optimiser::getNifOptBool()
{
    return nifOptBool;
}


bool Optimiser::getCreateBsaBool()
{
    return createBsaBool;
}


bool Optimiser::getrenameBsaBool()
{
    return renameBsaBool;
}


bool Optimiser::getAnimOptBool()
{
    return animOptBool;
}


void Optimiser::saveSettings()
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);

    settings.setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");
    settings.clear();

    settings.setValue("Mode", mode);
    settings.setValue("SelectedPath", userPath);
    settings.setValue("CreateBSA", createBsaBool);
    settings.setValue("ExtractBSA", extractBsaBool);
    settings.setValue("OptimiseBSA", bsaOptBool);
    settings.setValue("RenameBsa", renameBsaBool);
    settings.setValue("OptimiseMeshes", nifOptBool);
    settings.setValue("OptimiseTextures", textOptBool);
    settings.setValue("OptimiseAnimations", animOptBool);

}


void Optimiser::loadSettings()
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);

    settings.setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");

    mode = settings.value("Mode").toInt();
    userPath = settings.value("SelectedPath").toString();
    createBsaBool = settings.value("CreateBSA").toBool();
    extractBsaBool = settings.value("ExtractBSA").toBool();
    renameBsaBool = settings.value("RenameBsa").toBool();
    bsaOptBool = settings.value("OptimiseBSA").toBool();
    nifOptBool = settings.value("OptimiseMeshes").toBool();
    textOptBool = settings.value("OptimiseTextures").toBool();
    animOptBool = settings.value("OptimiseAnimations").toBool();

    if(!renameBsaBool)
        createBsaBool = false;
}
