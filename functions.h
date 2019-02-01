#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QDirIterator>
#include <QProcess>
#include <QDebug>
#include <mainwindow.h>
#include <QMessageBox>

extern QString modPath;

void processBsa(bool deleteBsa, QPlainTextEdit *log) //Extracts the BSA, and delete the BSA if DeleteBSA==true. WORKING
{
    log->appendPlainText("Processing BSA...\n");
    log->repaint();
    QDirIterator it(modPath);
    while (it.hasNext())
    {
        if(it.next().contains(".bsa"))
        {
            log->appendPlainText("BSA found ! Extracting...\n");
            log->repaint();
            QProcess bsarch;
            QStringList bsarchArgs;
            bsarchArgs << "unpack" << it.filePath() << it.path() ;
            bsarch.start("ressources/bsarch.exe", bsarchArgs);
            bsarch.waitForFinished();

            if(bsarch.readAllStandardOutput().contains("Done."))
            {
                log->appendPlainText("BSA successfully extracted.\n");
                log->repaint();
                if(deleteBsa)
                {
                    QFile::remove(it.filePath());
                    log->appendPlainText("BSA successfully deleted.\n");
                    log->repaint();
                }
            }
            else{
                log->appendHtml("<font color=\"Red\">An error occured during the extraction. Please extract it manually. The BSA was not deleted.</font>\n");
                log->repaint();
            }
        }
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

            texDiag.start("ressources/texdiag.exe", texdiagArg);
            texDiag.waitForFinished();

            if(texDiag.readAllStandardOutput().contains("compressed = no"))
            {
                QProcess texconv;

                QStringList texconvArg;
                texconvArg << "-nologo" << "-y" << "-m" << "0" << "-pow2" << "-if" << "FANT" << "-f" << "BC7_UNORM" << it.filePath();

                texconv.start("ressources/texconv.exe",  texconvArg);
                texconv.waitForFinished();
                log->appendPlainText("Uncompressed normal map found :\n" + it.fileName() + "\nCompressing...\n");
                log->repaint();
            }
        }
    }
    log->appendPlainText("All the textures were processed.\n");
    log->repaint();
}


void nifOpt(QPlainTextEdit *log) // Optimize the meshes if Nifscan report them. NOT WORKING
{
    log->appendPlainText("Processing meshes...\n");
    log->repaint();
    bool nifCopied=false; //Used to not copy twice the same mesh

    QString readLine;
    QString currentFile;
    QFile nifScan_file("release/ressources/NifScan.exe");
    QDir modPathDir(modPath);
    QProcess nifScan;

    if(nifScan_file.exists())
    {
        nifScan_file.copy("release/ressources/NifScan.exe", modPath + "/NifScan.exe");
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
                currentFile = QDir::cleanPath(readLine.simplified());

                log->appendPlainText("Processing : " + currentFile + "\n");
                nifCopied = false;
            }
            else if((readLine.contains("unsupported") || readLine.contains("not supported")) && nifCopied == false)
            {
                QString oldFile(modPath + "/" + currentFile);
                QString newFile(modPath + "/meshes_to_optimize/" + currentFile);



                if(!modPathDir.exists(modPath + "/meshes_to_optimize/" + currentFile.left(currentFile.lastIndexOf("/"))))
                {
                    modPathDir.mkpath(newFile.left(newFile.lastIndexOf("/")));
                    modPathDir.rename(oldFile, newFile);
                    nifCopied = true;
                }else
                {
                    log->appendHtml("<font color=\"Red\">Meshes_to_optimize folder already exists. Please remove it before resuming.</font>\n");
                }
            }
        }
        nifScan_file.remove(modPath + "/NifScan.exe");
        log->appendPlainText("All the meshes were processed.\n");
        log->appendHtml("<font color=\"Blue\">Please use SSE Nif Optimizer on the \"meshes_to_optimize\" folder. Then, move \\meshes_to_optimize\\meshes to \\meshes</font>\n");
        log->repaint();

    }else
    {
        log->appendHtml("<font color=\"Red\">Please ensure that NifScan.exe is located in the ressources folder</font>");
        log->repaint();
    }
}

#endif // FUNCTIONS_H
