/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MainOptimizer.h"

MainOptimizer::MainOptimizer()
{
    parseArguments();
    //Preparing logging
    QLogger::QLoggerManager *logManager = QLogger::QLoggerManager::getInstance();
    logManager->addDestination("log.html", QStringList() << "MainWindow" << "MainOptimizer" << "AnimationsOptimizer"
                               << "BsaOptimizer" << "FilesystemOperations" << "MeshesOptimizer" << "PluginsOperations"
                               << "TexturesOptimizer", logLevel);
    logManager->addDestination("errors.html", QStringList() << "Errors", logLevel);
}

int MainOptimizer::mainProcess() // Process the userPath according to all user options
{
    //Requirements

    if(!checkRequirements())
    {
        QLogger::QLog_Fatal("MainOptimizer", tr("The setup function did not run as expected. Exiting the process."));
        return 1;
    }

    if(options.fileType == DDS)
        processDds();
    else if(options.fileType == NIF)
        processNif();
    else if(options.fileType == TGA)
        processTga();
    else if(options.fileType == BSA)
        processBsa();
    else if(options.fileType == HKX)
        processHkx();
    else
        return false ; //TODO add error message

    FilesystemOperations::deleteEmptyDirectories(options.path);

    return 0;
}

bool MainOptimizer::checkRequirements()  //Checking if all the requirements are in the resources folder
{
    QStringList requirements {"NifScan.exe", "nifopt.exe", "texconv.exe", "texdiag.exe", "ListHeadParts.exe"};
    QFile havokFile(FilesystemOperations::findSkyrimDirectory() + "/Tools/HavokBehaviorPostProcess/HavokBehaviorPostProcess.exe");

    if(havokFile.exists() && !QFile(QCoreApplication::applicationDirPath() + "/resources/HavokBehaviorPostProcess.exe").exists())
        havokFile.copy(QCoreApplication::applicationDirPath() + "/resources/HavokBehaviorPostProcess.exe");

    else if(!havokFile.exists() && !QFile(QCoreApplication::applicationDirPath() + "/resources/HavokBehaviorPostProcess.exe").exists())
    {
        QLogger::QLog_Warning("MainOptimizer", tr("Havok Tool not found. Are you sure the Creation Kit is installed ? You can also put HavokBehaviorPostProcess.exe in the resources folder"));
        QLogger::QLog_Warning("Errors", tr("Havok Tool not found. Are you sure the Creation Kit is installed ? You can also put HavokBehaviorPostProcess.exe in the resources folder"));
    }

    if(options.bAnimationsOptimization)
        requirements << "HavokBehaviorPostProcess.exe";

    for (const auto& requirement : requirements)
    {
        QFile file(QCoreApplication::applicationDirPath() + "/resources/" + requirement);
        if(!file.exists())
        {
            QLogger::QLog_Error("MainOptimizer", requirement + tr(" not found. Cancelling."));
            QLogger::QLog_Error("Errors", requirement + tr(" not found. Cancelling."));
            return false;
        }
    }
    return true;
}

bool MainOptimizer::parseArguments()
{
    QCommandLineParser parser;

    parser.addHelpOption();
    parser.addPositionalArgument("file or folder", "The file or the folder to process, surrounded with quotes. Folders are only accepted when creating BSA");
    parser.addPositionalArgument("game", "Currently, only 'sse' is supported");
    parser.addPositionalArgument("File type", "Either 'DDS', 'NIF', 'TGA', 'BSA', 'HKX'");

    parser.addOptions({
                          {"dr", "Enables dry run"},
                          {"l", "Log level: from 0 (maximum) to 6", "value", "0"},
                          {"m", "Mesh processing level: 0 (default) to disable optimization, 1 for necessary optimization, "
                           "2 for medium optimization, 3 for full optimization.", "value", "0"},

                          {"t", "Texture processing level: 0 (default) to disable optimization, "
                           "1 for necessary optimization, 2 for full optimization.", "value", "0"},

                          {"a", "Enables animations processing"},
                          {"mh", "Enables headparts detection and processing"},
                          {"be", "Enables BSA extraction."},
                          {"bc", "Enables BSA creation."},
                          {"bd", "Enables deletion of BSA backups."},
                          {"bo", "NOT WORKING. Enables BSA optimization. The files inside the "
                           "BSA will be extracted to memory and processed according to the provided settings "},
                      });

    parser.process(QCoreApplication::arguments());

    options.path = QDir::cleanPath(parser.positionalArguments().at(0));

    QString game = parser.positionalArguments().at(1);

    if(game == "sse")
        options.game = SSE;
    else
        return false ; //TODO add error message

    QString fileType = parser.positionalArguments().at(2);

    if(fileType == "DDS")
        options.fileType = DDS;
    else if(fileType == "NIF")
        options.fileType = NIF;
    else if(fileType == "TGA")
        options.fileType = TGA;
    else if(fileType == "BSA")
        options.fileType = BSA;
    else if(fileType == "HKX")
        options.fileType = HKX;
    else
        return false ; //TODO add error message

    options.bDryRun = parser.isSet("dr");

    if(parser.isSet("l"))
        options.iLogLevel = parser.value("l").toInt();

    if(parser.isSet("m"))
        options.iMeshesOptimizationLevel = parser.value("m").toInt();

    if(parser.isSet("t"))
        options.iTexturesOptimizationLevel = parser.value("t").toInt();

    options.bAnimationsOptimization = parser.isSet("a");

    options.bMeshesHeadparts = parser.isSet("mh");

    options.bBsaCreate = parser.isSet("be");
    options.bBsaExtract = parser.isSet("bc");
    options.bBsaDeleteBackup = parser.isSet("bl");
    options.bBsaOptimizeAssets = parser.isSet("bo");

    return true;
}

void MainOptimizer::processBsa()
{
    if(options.bDryRun)
        return; //TODO if "dry run" run dry run on the assets in the BSA

    BsaOptimizer bsaOpt;

    if(options.bBsaExtract)
    {
        QLogger::QLog_Note("MainOptimizer", tr("BSA found ! Extracting...(this may take a long time, do not force close the program): ") + options.path);
        bsaOpt.extract(options.path, options.bBsaDeleteBackup);
    }

    /*TODO if(options.bBsaCreate)
    {
        QLogger::QLog_Info("MainOptimizer", tr("Creating BSA..."));

        bsaOptimizer.packAll(modpathDir);
        PluginsOperations::makeDummyPlugins(modpathDir);
    }*/

    //TODO if(options.bBsaOptimizeAssets)
}

void MainOptimizer::processDds()
{
    if(options.iTexturesOptimizationLevel >=2 && options.bDryRun)
    {
        if(TexturesOptimizer::isCompressed(options.path))
            QLogger::QLog_Note("MainOptimizer", options.path + tr(" would be compressed to BC7"));
    }
    else if(options.iTexturesOptimizationLevel >=2 && !options.bDryRun)
        TexturesOptimizer::convertToBc7IfUncompressed(options.path);
}

void MainOptimizer::processHkx()
{
    if(options.bAnimationsOptimization && options.bDryRun)
        QLogger::QLog_Note("MainOptimizer", options.path + tr(" would be ported to SSE"));
    else if(options.bAnimationsOptimization)
        AnimationsOptimizer::optimize(options.path);
}

void MainOptimizer::processNif()
{
    MeshesOptimizer opt(options.bMeshesHeadparts, options.iMeshesOptimizationLevel);
    //TODO Scan meshes opt.scan(options.path);

    if(options.iMeshesOptimizationLevel >=1 && options.bDryRun)
        opt.dryOptimize(options.path);
    else if(options.iMeshesOptimizationLevel >=1 && !options.bDryRun)
        opt.optimize(options.path);
}

void MainOptimizer::processTga()
{
    if(options.iTexturesOptimizationLevel >=1 && options.bDryRun)
        QLogger::QLog_Note("MainOptimizer", options.path + tr(" would be converted to DDS"));
    else if(options.iTexturesOptimizationLevel >=1)
        TexturesOptimizer::convertTgaToDds(options.path);
}
