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

bool mainFunc(bool extractBsa, bool deleteBsa, bool textures, bool meshes, QPlainTextEdit *log)
{
    qDebug() << "nifOpt" << meshes << endl << "delete bsa" << deleteBsa << endl << "textOpt" << textures << endl << "extract BSA" << extractBsa << endl;
    log->clear();
    log->appendPlainText("Beginning...\n");

    if(extractBsa)
    {
        processBsa(deleteBsa, log);
    }
    if(textures)
    {
        textOpt(log);
    }
    if(meshes)
    {
        nifOpt(log);
    }
    return true;
}



void processBsa(bool deleteBsa, QPlainTextEdit *log) //Extracts the BSA, and delete the BSA if DeleteBSA==true. WORKING
{
    log->appendPlainText("Processing BSA...\n");
    QDirIterator it(modPath);
    while (it.hasNext())
    {
        if(it.next().contains(".bsa"))
            {
                log->appendPlainText("BSA found ! Extracting...\n");
                QProcess bsarch;
                QStringList bsarchArgs;
                bsarchArgs << "unpack" << it.filePath() << it.path() ;
                bsarch.start("ressources/bsarch.exe", bsarchArgs);
                bsarch.waitForFinished();
                if(bsarch.readAllStandardOutput().contains("Done."))
                {
                    log->appendPlainText("BSA successfully extracted.\n");
                    if(deleteBsa)
                    {
                        QFile::remove(it.filePath());
                        log->appendPlainText("BSA successfully deleted.\n");
                    }
                }
                else{
                    log->appendPlainText("An error occured during the extraction. Please extract it manually. The BSA was not deleted.\n");
                }
            }
    }
}




void textOpt(QPlainTextEdit *log)
{
    log->appendPlainText("Processing textures...\n");
    QDirIterator it(modPath, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        if(it.next().contains("_n.dds"))
        {
            QProcess texDiag;

            texDiag.start("ressources/texdiag.exe info " + it.filePath());
            texDiag.waitForFinished();

            if(texDiag.readAllStandardOutput().contains("compressed = no"))
            {
                QProcess texConv;

                texConv.start("ressources/texconv.exe -nologo -y -m 0 -pow2 -if FANT -f BC7_UNORM " + it.filePath());
                texConv.waitForFinished();
                log->appendPlainText("Uncompressed normal map found. Compressing...\n");
            }
        }
    }
    log->appendPlainText("All the textures were processed");
}


void nifOpt(QPlainTextEdit *log)
{
    log->appendPlainText("Processing meshes...");
    bool nifCopied=false;



    QString readLine;
    QString currentFile;
    QDir modPathDir(modPath);

    QProcess nifScan;
    nifScan.setWorkingDirectory(modPath);
    nifScan.start("ressources/NifScan.exe");
    nifScan.waitForFinished();

    while(nifScan.canReadLine())
    {
        readLine=QString::fromLocal8Bit(nifScan.readLine());
        if(readLine.contains("meshes\\"))
        {
            QDir::cleanPath(readLine);
            currentFile = readLine;
            nifCopied = false;
        }
        else if((readLine.contains("unsupported") || readLine.contains("not supported")) && nifCopied == false)
        {
            //            modPathDir.mkpath(modPath + "/meshes_to_optimize/" + currentFile);
            modPathDir.rename(modPathDir.absoluteFilePath(currentFile), modPath + "/meshes_to_optimize/" + currentFile);
            nifCopied = true;
        }
    }
    log->appendPlainText("All the meshes were processed. Please use SSE Nif Optimizer on the \"meshes_to_optimize\" folder ");
}

/*

    system(std::string("\".\\SSE NIF Optimizer.exe\"").c_str());
    std::getchar();
    system(std::string("Robocopy.exe /s /move /np \"" + modPath + "meshes_to_optimize\\meshes\" \"" + modPath + "meshes\"").c_str());
    std::ifstream files(".\\copied.txt");
    while (getline(files,readLine))
    {
        fs::remove(modPath + readLine);
    }
    fs::remove(modPath + "NifScan_errors.txt");
    fs::remove(modPath + "missing_textures.log");
    fs::remove_all(modPath + "meshes_to_optimize");
    return 0;
}*/

#endif // FUNCTIONS_H
