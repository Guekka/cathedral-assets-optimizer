#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QDirIterator>
#include <QProcess>
#include <QDebug>
#include <mainwindow.h>

extern QString modPath;

void processBsa(bool deleteBsa, QPlainTextEdit *log);
void textOpt(QPlainTextEdit *log);
void nifOpt(QPlainTextEdit *log);

bool mainFunc(QCheckBox *extractBsaCheckbox, QCheckBox *deleteBsaCheckbox, QCheckBox *textOptCheckbox, QCheckBox *nifOptCheckbox, QPlainTextEdit *log)
{
    log->clear();
    log->appendPlainText("Beginning...\n");
    log->repaint();

    if(extractBsaCheckbox->isChecked())
    {
        processBsa(deleteBsaCheckbox->isChecked(), log);
    }
    if(textOptCheckbox->isChecked())
    {
        textOpt(log);
    }
    if(nifOptCheckbox->isChecked())
    {
        nifOpt(log);
    }
    log->appendPlainText("Completed.\n");
    log->repaint();
    return true;
}



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
                    log->appendHtml("<font color=\"DeepPink\">An error occured during the extraction. Please extract it manually. The BSA was not deleted.</font>\n");
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
                texconvArg << "-nologo" << "-y" << "-m" << "0" << "-pow2" << "-if" << "FANT" << "-f" << "BC7_UNORM" << it.filePath();// << "-o" << it.path();

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
    QFile nifScan_file("debug/ressources/NifScan.exe"); //Trying to understand why I need the "debug"
    QFile currentFile_file;
    QDir modPathDir(modPath);
    QProcess nifScan;

    if(nifScan_file.exists())
    {
        nifScan_file.copy("debug/ressources/NifScan.exe", modPath + "/NifScan.exe");

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
                currentFile = QDir::cleanPath(readLine);
                log->appendPlainText("Processing : " + currentFile + "\n");
                nifCopied = false;
            }
            else if((readLine.contains("unsupported") || readLine.contains("not supported")) && nifCopied == false)
            {
                QString oldFile(QDir::cleanPath(modPath + "/" + currentFile));
                QString newFile(QDir::cleanPath(modPath + "/meshes_to_optimize/" + currentFile));

                oldFile.resize(oldFile.size()-2);
                newFile.resize(newFile.size()-2);

                QFile oldFile_file(oldFile);
                QFile newFile_file(newFile);

                modPathDir.mkpath(newFile);

                qDebug() << newFile_file.isWritable() << newFile_file.fileName() ;

                qDebug() << oldFile_file.copy(oldFile, newFile);


                nifCopied = true;
            }
        }
        nifScan_file.remove(modPath + "/NifScan.exe");
        log->appendPlainText("All the above meshes were processed. Please use SSE Nif Optimizer on the \"meshes_to_optimize\" folder \n");
        log->repaint();
    }else
    {
        log->appendHtml("<font color=\"DeepPink\">Please ensure that NifScan.exe is located in the ressources folder</font>");
        log->repaint();
    }
}

#endif // FUNCTIONS_H
