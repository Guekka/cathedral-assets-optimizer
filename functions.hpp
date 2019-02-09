#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QDirIterator>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include <QPlainTextEdit>

extern QString modPath;

void extractBsa(bool deleteBsa, bool createBsa, QPlainTextEdit *log);
void createBsa(QPlainTextEdit *log);
void textOpt(QPlainTextEdit *log);
void nifOpt(QPlainTextEdit *log);

void extractBsa(bool deleteBsa, QPlainTextEdit *log) //Extracts the BSA, and delete the BSA if DeleteBSA==true.
{
    QString espName;
    QProcess bsarch;
    QStringList bsarchArgs;
    QDir modPathDir(modPath);

    log->appendPlainText("Processing BSA...\n");
    log->repaint();

    QDirIterator it(modPath);

    while (it.hasNext())
    {
        if(it.next().contains(".bsa"))
        {
            log->appendPlainText("BSA found ! Extracting...\n");
            log->repaint();

            bsarchArgs.clear();
            bsarchArgs << "unpack" << it.filePath() << it.path() ;
            bsarch.start("resources/bsarch.exe", bsarchArgs);
            bsarch.waitForFinished();

            if(bsarch.readAllStandardOutput().contains("Done."))
            {
                log->appendHtml("<font color=Blue>BSA successfully extracted.</font>\n\n");
                log->repaint();
                if(deleteBsa)
                {
                    QFile::remove(it.filePath());
                    log->appendPlainText("BSA successfully deleted.\n");
                    log->repaint();
                }
            }
            else{
                log->appendHtml("<font color=Red>An error occured during the extraction. Please extract it manually. The BSA was not deleted.</font>\n");
                log->repaint();
            }
        }
    }
}

void createBsa(QPlainTextEdit *log) //Once all the optimizations are done, create a new BSA
{
    log->appendPlainText("Creating a new BSA...\n");
    log->repaint();

    bool hasEsp=false;
    bool hasSound=false;

    QDirIterator it(modPath);
    QProcess bsarch;
    QStringList bsarchArgs;
    QString espName;
    QDir modPathDir(modPath);


    modPathDir.mkdir("data");

    while (it.hasNext())
    {
        if(it.fileName().contains(".es"))
        {
            espName=it.fileName();
            log->appendPlainText("Esp found.\n");
            hasEsp=true;
            it.next();
        }
        else if (!hasEsp)
        {
            espName="dummy_plugin.esp";
            it.next();
        }
        else
        {
            it.next();
        }



        if(it.fileName() == "textures")
        {
            log->appendPlainText("Textures folder found. Compressing...\n");
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

            log->appendHtml("<font color=Blue> Textures BSA successfully compressed.</font>\n");
        }

        else if(it.fileName() == "sound")
        {
            hasSound=true;
            modPathDir.rename(it.filePath(), "data/" + it.fileName());
        }
        else if(it.fileName() == "meshes" || it.fileName() == "seq" || it.fileName() == "scripts" || it.fileName() == "dialogueviews")
        {
            modPathDir.rename(it.filePath(), "data/" + it.fileName());
        }
    }

    QString bsaName = modPath + "/data/" + espName.chopped(4) + ".bsa";
    QString folderName = modPath + "/data";
    bsarchArgs.clear();
    bsarchArgs << "pack" << folderName << bsaName << "-sse" << "-share";

    log->appendPlainText("Compressing...\n");

    if (!hasSound) //Compressing BSA breaks sounds
    {
        bsarchArgs << "-z";
    }

    bsarch.start("resources/bsarch.exe", bsarchArgs);
    bsarch.waitForFinished();

    if(bsarch.readAllStandardOutput().contains("Done."))
    {
        log->appendHtml("<font color=Blue> BSA successfully compressed.</font>\n");
        modPathDir.rename(bsaName, modPath + "/" + espName.chopped(4) + ".bsa");
        modPathDir.setPath(modPath + "/data");
        modPathDir.removeRecursively();
    }
}


void textOpt(QPlainTextEdit *log) //Compress the nmaps to BC7 if they are uncompressed. WORKING
{
    log->appendPlainText("Processing textures...\n");
    QDirIterator it(modPath, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        if(it.next().contains("_n.dds"))
        {
            log->appendPlainText("Currently scanning :\n" + it.fileName() + "\n");
            log->repaint();
            QProcess texDiag;

            QStringList texdiagArg;
            texdiagArg << "info" << it.filePath();

            texDiag.start("resources/texdiag.exe", texdiagArg);
            texDiag.waitForFinished();

            if(texDiag.readAllStandardOutput().contains("compressed = no"))
            {
                QProcess texconv;

                QStringList texconvArg;
                texconvArg << "-nologo" << "-y" << "-m" << "0" << "-pow2" << "-if" << "FANT" << "-f" << "BC7_UNORM" << it.filePath();

                texconv.start("resources/texconv.exe",  texconvArg);
                texconv.waitForFinished();
                log->appendPlainText("Uncompressed normal map found :\n" + it.fileName() + "\nCompressing...\n");
                log->repaint();
            }
        }
    }
    log->appendPlainText("All the textures were processed.\n");
    log->repaint();
}


void nifOpt(QPlainTextEdit *log) // Optimize the meshes if Nifscan report them.
{
    log->appendPlainText("Processing meshes...\n");
    log->repaint();

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
                log->appendPlainText("\nProcessing : " + currentFile);
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
                    log->appendHtml("<font color=\"Blue\"> Mesh ported to SSE\n");
                    optFile.remove(currentFile);
                    optFile.rename(currentFile);
                }else
                {
                    log->appendHtml("<font color=\"Red\"> Error while porting the mesh\n");
                }
            }
        }
        nifScan_file.remove(modPath + "/NifScan.exe");
        log->appendHtml("<font color=Blue>All the meshes were processed.</font>\n");
        log->repaint();

    }else
    {
        log->appendHtml("<font color=\"Red\">Please ensure that NifScan.exe is located in the resources folder</font>\n");
        log->repaint();
    }
}

#endif // FUNCTIONS_H
