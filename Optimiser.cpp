#include "Optimiser.hpp"

Optimiser::Optimiser(QString mod, QPlainTextEdit* textEdit) : modPath(mod), log(textEdit){}



void Optimiser::extractBsa(bool deleteBsa) //Extracts the BSA, and delete the BSA if DeleteBSA==true.
{
    QString espName;
    QProcess bsarch;
    QStringList bsarchArgs;
    QDir modPathDir(modPath);

    log.appendPlainText(QPlainTextEdit::tr("Processing BSA...\n"));
    log.repaint();

    QDirIterator it(modPath);

    while (it.hasNext())
    {
        if(it.next().contains(".bsa"))
        {
            log.appendPlainText(QPlainTextEdit::tr("BSA found ! Extracting...\n"));
            log.repaint();

            bsarchArgs.clear();
            bsarchArgs << "unpack" << it.filePath() << it.path() ;
            bsarch.start("resources/bsarch.exe", bsarchArgs);
            bsarch.waitForFinished();

            if(bsarch.readAllStandardOutput().contains("Done."))
            {
                log.appendHtml(QPlainTextEdit::tr("<font color=Blue>BSA successfully extracted.</font>\n\n"));
                log.repaint();
                if(deleteBsa)
                {
                    QFile::remove(it.filePath());
                    log.appendPlainText(QPlainTextEdit::tr("BSA successfully deleted.\n"));
                    log.repaint();
                }
            }
            else{
                log.appendHtml(QPlainTextEdit::tr("<font color=Red>An error occured during the extraction. Please extract it manually. The BSA was not deleted.</font>\n"));
                log.repaint();
            }
        }
    }
}




void Optimiser::textOpt() //Compress the nmaps to BC7 if they are uncompressed. WORKING
{
    log.appendPlainText(QPlainTextEdit::tr("Processing textures...\n"));
    log.repaint();

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

        nifScan_file.copy("resources/NifScan.exe", modPath + "/NifScan.exe");
        nifScan.setWorkingDirectory(modPath);

        nifScan.start(modPath + "/NifScan.exe", NifscanArgs);
        nifScan.waitForFinished();
    }

    while (it.hasNext())
    {
        if(it.fileName().contains("_n.dds"))
        {
            log.appendPlainText(QPlainTextEdit::tr("Currently scanning :\n") + it.fileName() + "\n");
            log.repaint();

            texdiagArg << "info" << it.filePath();

            texDiag.start("resources/texdiag.exe", texdiagArg);
            texDiag.waitForFinished();

            if(texDiag.readAllStandardOutput().contains("compressed = no"))
            {
                texconvArg.clear();
                texconvArg << "-nologo" << "-y" << "-m" << "0" << "-pow2" << "-if" << "FANT" << "-f" << "BC7_UNORM" << it.filePath();

                texconv.start("resources/texconv.exe",  texconvArg);
                texconv.waitForFinished();
                log.appendPlainText(QPlainTextEdit::tr("Uncompressed normal map found:\n") + it.fileName() + QPlainTextEdit::tr("\nCompressing...\n"));
                log.repaint();
            }
            it.next();
        }
        else if(it.fileName().contains(".tga"))
        {

            log.appendPlainText(QPlainTextEdit::tr("TGA file found: \n") + it.fileName() + QPlainTextEdit::tr("\nCompressing...\n"));
            log.repaint();

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
    log.repaint();
}


void Optimiser::nifOpt() // Optimize the meshes if Nifscan report them.
{
    log.appendPlainText(QPlainTextEdit::tr("Processing meshes...\n"));
    log.repaint();

    bool meshProcessed=false; //To avoid processing twice the same mesh

    QString readLine;
    QString currentFile;

    QFile nifScan_file("resources/NifScan.exe");
    QFile optFile;

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

            if(readLine.contains("meshes\\"))
            {
                currentFile = QDir::cleanPath(modPath + "/" + readLine.simplified());
                log.appendPlainText(QPlainTextEdit::tr("\nProcessing : ") + currentFile);
                meshProcessed=false;
            }
            else if((readLine.contains("unsupported") || readLine.contains("not supported")) && !meshProcessed)
            {
                meshProcessed=true;
                nifOpt.start("resources/nifopt.exe \"" + currentFile + "\"");
                nifOpt.waitForFinished();

                optFile.setFileName(currentFile.chopped(4) + ".opt.nif");

                if(optFile.exists())
                {
                    log.appendHtml(QPlainTextEdit::tr("<font color=\"Blue\"> Mesh ported to SSE\n"));
                    optFile.remove(currentFile);
                    optFile.rename(currentFile);
                }else
                {
                    log.appendHtml(QPlainTextEdit::tr("<font color=\"Red\"> Error while porting the mesh\n"));
                }
            }
        }
        nifScan_file.remove(modPath + "/NifScan.exe");
        log.appendHtml(QPlainTextEdit::tr("<font color=Blue>All the meshes were processed.</font>\n"));
        log.repaint();

    }else
    {
        log.appendHtml(QPlainTextEdit::tr("<font color=\"Red\">Please ensure that NifScan.exe is located in the resources folder</font>\n"));
        log.repaint();
    }
}

void Optimiser::createBsa() //Once all the optimizations are done, create a new BSA
{
    log.appendPlainText("Creating a new BSA...\n");
    log.repaint();

    bool hasEsp=false;
    bool hasSound=false;

    QDirIterator it1(modPath), it(modPath);
    QProcess bsarch;
    QStringList bsarchArgs;
    QString espName;
    QDir modPathDir(modPath);

    QStringList assetsFolder;
    assetsFolder << "meshes" << "seq" << "scripts" << "dialogueviews";

    modPathDir.mkdir("data");

    while (it1.hasNext())
    {
        it1.next();

        if(it1.fileName().contains(".es"))
        {
            espName=it1.fileName();
            log.appendPlainText(QPlainTextEdit::tr("Esp found.\n"));
            hasEsp=true;
        }
        if (!hasEsp)
        {
            espName="dummy_plugin.esp";
        }
    }

    while (it.hasNext())
    {
        it.next();

        if(it.fileName().toLower() == "textures")
        {
            log.appendPlainText(QPlainTextEdit::tr("Textures folder found. Compressing...\n"));
            log.repaint();

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

            log.appendHtml(QPlainTextEdit::tr("<font color=Blue> Textures BSA successfully compressed.</font>\n"));
        }

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

    log.appendPlainText(QPlainTextEdit::tr("Compressing...\n"));

    if (!hasSound) //Compressing BSA breaks sounds
    {
        bsarchArgs << "-z";
    }

    bsarch.start("resources/bsarch.exe", bsarchArgs);
    bsarch.waitForFinished();

    if(bsarch.readAllStandardOutput().contains("Done."))
    {
        log.appendHtml(QPlainTextEdit::tr("<font color=Blue> BSA successfully compressed.</font>\n"));
        modPathDir.rename(bsaName, modPath + "/" + espName.chopped(4) + ".bsa");
        modPathDir.setPath(modPath + "/data");
        modPathDir.removeRecursively();
    }
}


void Optimiser::animOpt()
{
    log.appendPlainText(QPlainTextEdit::tr("Processing animations..."));

    QSettings SkyrimReg("HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Bethesda Softworks\\Skyrim Special Edition", QSettings::NativeFormat);
    QDir SkyrimDir = QDir::cleanPath(SkyrimReg.value("Installed Path").toString());

    QFile havokFile(SkyrimDir.path() + "/Tools/HavokBehaviorPostProcess/HavokBehaviorPostProcess.exe");

    if(havokFile.exists())
    {
        log.appendPlainText(QPlainTextEdit::tr("Havok tool found. Processing animations...\n"));

        QProcess havokProcess;
        QStringList havokArgs;

        QDirIterator it(modPath);

        while (it.hasNext())
        {
            if(it.next().contains(".hkx"))
            {
                log.appendPlainText(QPlainTextEdit::tr("Current file: ") + it.filePath() + QPlainTextEdit::tr("\nProcessing...\n"));
                log.repaint();

                havokArgs << "--platformamd64" << it.filePath() << it.filePath();

                log.appendHtml(QPlainTextEdit::tr("<font color=Blue>Animation successfully ported.</font>\n\n"));
            }

        }
    }
    else
    {
        log.appendHtml(QPlainTextEdit::tr("<font color=Red>Havok Tool not found. Are you sure the Creation Kit is installed ?</font>\n"));
    }

}
