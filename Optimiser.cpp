#include "Optimiser.hpp"

Optimiser::Optimiser(QString mod, QPlainTextEdit* textEdit) : modPath(std::move(mod)), log(textEdit){}


void Optimiser::extractBsa() //Extracts the BSA
{
    QProcess bsarch;
    QStringList bsarchArgs;

    log->appendPlainText(QPlainTextEdit::tr("Processing BSA...\n"));
    log->repaint();

    QDirIterator it(modPath);

    while (it.hasNext())
    {
        if(it.next().contains(".bsa"))
        {
            log->appendPlainText(QPlainTextEdit::tr("BSA found ! Extracting...\n"));
            log->repaint();

            bsarchArgs.clear();
            bsarchArgs << "unpack" << it.filePath() << it.path() ;
            bsarch.start("resources/bsarch.exe", bsarchArgs);
            bsarch.waitForFinished();

            if(bsarch.readAllStandardOutput().contains("Done."))
            {
                log->appendHtml(QPlainTextEdit::tr("<font color=Blue>BSA successfully extracted.</font>\n\n"));
                log->repaint();
            }
            else{
                log->appendHtml(QPlainTextEdit::tr("<font color=Red>An error occured during the extraction. Please extract it manually. The BSA was not deleted.</font>\n"));
                log->repaint();
            }
        }
    }
}

void Optimiser::deleteBsa() //Deletes the BSA.
{
    QStringList bsarchArgs;

    log->appendPlainText(QPlainTextEdit::tr("Processing BSA...\n"));
    log->repaint();

    QDirIterator it(modPath);

    while (it.hasNext())
    {
        if(it.next().contains(".bsa"))
        {
            QFile::remove(it.filePath());
            log->appendPlainText(QPlainTextEdit::tr("BSA successfully deleted.\n"));
            log->repaint();
        }
    }
}


void Optimiser::textOpt() //Compress the nmaps to BC7 if they are uncompressed.
{
    log->appendPlainText(QPlainTextEdit::tr("Processing textures...\n"));
    log->repaint();

    QFile nifScan_file("resources/NifScan.exe");
    QProcess nifScan;
    QStringList NifscanArgs;

    QProcess texDiag;
    QStringList texdiagArg;

    QProcess texconv;
    QStringList texconvArg;

    QDirIterator it(modPath, QDirIterator::Subdirectories);

    if(nifScan_file.exists())
    {
        NifscanArgs << "-fixdds";

        QFile::copy("resources/NifScan.exe", modPath + "/NifScan.exe");
        nifScan.setWorkingDirectory(modPath);

        nifScan.start(modPath + "/NifScan.exe", NifscanArgs);
        nifScan.waitForFinished();
    }

    while (it.hasNext())
    {
        if(it.fileName().contains("_n.dds"))
        {
            log->appendPlainText(QPlainTextEdit::tr("Currently scanning :\n") + it.fileName() + "\n");
            log->repaint();

            texdiagArg << "info" << it.filePath();

            texDiag.start("resources/texdiag.exe", texdiagArg);
            texDiag.waitForFinished();

            if(texDiag.readAllStandardOutput().contains("compressed = no"))
            {
                texconvArg.clear();
                texconvArg << "-nologo" << "-y" << "-m" << "0" << "-pow2" << "-if" << "FANT" << "-f" << "BC7_UNORM" << it.filePath();

                texconv.start("resources/texconv.exe",  texconvArg);
                texconv.waitForFinished();
                log->appendPlainText(QPlainTextEdit::tr("Uncompressed normal map found:\n") + it.fileName() + QPlainTextEdit::tr("\nCompressing...\n"));
                log->repaint();
            }
            it.next();
        }
        else if(it.fileName().contains(".tga"))
        {

            log->appendPlainText(QPlainTextEdit::tr("TGA file found: \n") + it.fileName() + QPlainTextEdit::tr("\nCompressing...\n"));
            log->repaint();

            texconvArg.clear();
            texconvArg << "-nologo" << "-m" << "0" << "-y" << "-pow2" << "-if" << "FANT" << "-f" << "R8G8B8A8_UNORM" << it.filePath();
            texconv.start("resources/texconv.exe",  texconvArg);
            texconv.waitForFinished();

            QFile tga(it.filePath());
            tga.remove();

            it.next();
        }
        else
        {
            it.next();
        }
    }
    log->repaint();
}


void Optimiser::nifOpt() // Optimise the meshes if Nifscan report them.
{
    log->appendPlainText(QPlainTextEdit::tr("Processing meshes...\n"));
    log->repaint();

    bool meshProcessed=false; //To avoid processing twice the same mesh

    QString readLine;
    QString currentFile;

    QFile nifScan_file("resources/NifScan.exe");

    QProcess nifScan;
    QProcess nifOpt;

    if(nifScan_file.exists())
    {
        nifScan_file.copy("resources/NifScan.exe", modPath + "/NifScan.exe");
        nifScan.setReadChannel(QProcess::StandardOutput);
        nifScan.setProgram(modPath + "/NifScan.exe");
        nifScan.setWorkingDirectory(modPath);

        nifScan.start();
        nifScan.waitForFinished();

        while(nifScan.canReadLine())
        {
            readLine=QString::fromLocal8Bit(nifScan.readLine());


            if(readLine.toLower().contains("meshes\\"))
            {
                currentFile = QDir::cleanPath(modPath + "/" + readLine.simplified());
                log->appendPlainText(QPlainTextEdit::tr("\nProcessing : ") + currentFile);
                meshProcessed=false;
            }
            else if((readLine.contains("unsupported") || readLine.contains("not supported")) && !meshProcessed)
            {
                meshProcessed=true;
                nifOpt.start("resources/nifopt.exe \"" + currentFile + "\" -head 0");
                nifOpt.waitForFinished();

                //   log->appendHtml(QPlainTextEdit::tr("<font color=\"Blue\"> Mesh ported to SSE\n"));
                //   log->appendHtml(QPlainTextEdit::tr("<font color=\"Red\"> Error while porting the mesh\n"));
            }
            if(readLine.toLower().contains("facegendata"))
            {
                nifOpt.start("resources/nifopt.exe \"" + currentFile + "\" -head 1");
                nifOpt.waitForFinished();
            }
        }
        nifScan_file.remove(modPath + "/NifScan.exe");
        log->appendHtml(QPlainTextEdit::tr("<font color=Blue>All the meshes were processed.</font>\n"));
        log->repaint();

    }else
    {
        log->appendHtml(QPlainTextEdit::tr("<font color=\"Red\">Please ensure that NifScan.exe is located in the resources folder</font>\n"));
        log->repaint();
    }
}


void Optimiser::createBsa() //Once all the optimizations are done, create a new BSA
{
    log->appendPlainText("Creating a new BSA...\n");
    log->repaint();

    bool hasSound=false;

    QDirIterator it(modPath);
    QProcess bsarch;
    QStringList bsarchArgs;
    QString espName = findEspName();
    QDir modPathDir(modPath);

    QStringList assetsFolder;
    assetsFolder << "meshes" << "seq" << "scripts" << "dialogueviews";

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

    log->appendPlainText(QPlainTextEdit::tr("Compressing...\n"));

    if (!hasSound) //Compressing BSA breaks sounds
    {
        bsarchArgs << "-z";
    }

    bsarch.start("resources/bsarch.exe", bsarchArgs);
    bsarch.waitForFinished();

    if(bsarch.readAllStandardOutput().contains("Done."))
    {
        log->appendHtml(QPlainTextEdit::tr("<font color=Blue> BSA successfully compressed.</font>\n"));
        modPathDir.rename(bsaName, modPath + "/" + espName.chopped(4) + ".bsa");
        modPathDir.setPath(modPath + "/data");
        modPathDir.removeRecursively();
    }
}


void Optimiser::createTexturesBsa()
{
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
            log->appendPlainText(QPlainTextEdit::tr("Textures folder found. Compressing...\n"));
            log->repaint();

            QString tBsaName= it.path() + "/" + espName.chopped(4) + " - Textures.bsa";
            QString tFolder = it.path() + "/temp_t";

            modPathDir.mkdir("temp_t");

            modPathDir.rename(it.filePath(), "temp_t/" + it.fileName());

            bsarchArgs.clear();
            bsarchArgs << "pack" << tFolder << tBsaName << "-sse" << "-z" << "-share";

            bsarch.start("resources/bsarch.exe", bsarchArgs);
            bsarch.waitForFinished();

            modPathDir.setPath(modPath + "/temp_t/");
            modPathDir.removeRecursively();

            log->appendHtml(QPlainTextEdit::tr("<font color=Blue> Textures BSA successfully compressed.</font>\n"));
        }
    }
}

void Optimiser::animOpt() //Uses Bethesda Havok Tool to port animations
{
    log->appendPlainText(QPlainTextEdit::tr("Processing animations...\n"));
    log->repaint();

    QFile havokFile(findSkyrimDir() + "/Tools/HavokBehaviorPostProcess/HavokBehaviorPostProcess.exe");

    if(havokFile.exists())
    {
        log->appendPlainText(QPlainTextEdit::tr("Havok tool found. Processing animations...\n"));

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
                havokProcess.waitForFinished();

                log->appendHtml(QPlainTextEdit::tr("<font color=Blue>Animation successfully ported.</font>\n\n"));
            }

        }
    }
    else
    {
        log->appendHtml(QPlainTextEdit::tr("<font color=Red>Havok Tool not found. Are you sure the Creation Kit is installed ?</font>\n"));
    }

}

bool Optimiser::setModPath(const QString& path)
{
    QDir modDir(path);
    if(modDir.exists())
    {
        modPath = path;
        return true;
    }

    return false;

}

QString Optimiser::getmodPath() const
{
    return modPath;
}


QString Optimiser::findSkyrimDir() //Find Skyrim dir using Registry keys
{
    QSettings SkyrimReg(R"(HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Bethesda Softworks\Skyrim Special Edition)", QSettings::NativeFormat);
    QDir SkyrimDir = QDir::cleanPath(SkyrimReg.value("Installed Path").toString());
    return SkyrimDir.path();
}


QString Optimiser::findEspName()
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

/*bool Optimiser::setLog(QPlainTextEdit *textEdit)
{
    if(textEdit != nullptr)
    {
        log = textEdit;
        return true;
    }
    return false;
}*/
