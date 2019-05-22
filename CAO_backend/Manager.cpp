/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Manager.h"

Manager::Manager()
{
    if(QCoreApplication::arguments().count() >= 2)
        parseArguments();

    readIni();

    if(!checkRequirements())
    {
        QLogger::QLog_Fatal("MainOptimizer", tr("The requirements were not found. Exiting the process."));
        throw std::runtime_error("Error while checking the requirements");
    }

    if(!checkSettings())
    {
        QLogger::QLog_Fatal("MainOptimizer", tr("The settings were not read correctly. Exiting the process."));
        throw std::runtime_error("Error while reading settings");
    }

    listDirectories();
    listFiles();

    //Preparing logging
    QLogger::QLoggerManager *logManager = QLogger::QLoggerManager::getInstance();
    logManager->addDestination("log.html", QStringList() << "MainWindow" << "MainOptimizer" << "AnimationsOptimizer"
                               << "BsaOptimizer" << "FilesystemOperations" << "MeshesOptimizer" << "PluginsOperations"
                               << "TexturesOptimizer", static_cast<QLogger::LogLevel>(options.iLogLevel));
    logManager->addDestination("errors.html", QStringList() << "Errors", static_cast<QLogger::LogLevel>(options.iLogLevel));

    //Creating processes

    for(int i = 0; i<4; ++i)
        processes << new MainOptimizer(options);
}

Manager::~Manager()
{
    for(auto process : processes)
        delete process;
}

void Manager::listDirectories()
{
    modsToProcess.clear();

    if(mode == OptimizationMode::singleMod)
        modsToProcess << userPath;

    else if(mode == OptimizationMode::severalMods)
    {
        QDir dir(userPath);
        for(auto subDir : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
        {
            if(!subDir.contains("separator")) //Separators are empty directories used by MO2
                modsToProcess << dir.filePath(subDir);
        }
    }
}

void Manager::printProgress()
{
    int progress = (completedFilesWeight / filesWeight) * 100;
    QTextStream(stdout) << progress;
}

void Manager::listFiles()
{
    filesWeight = 0;
    files.clear();

    for(auto subDir : modsToProcess)
    {
        QDirIterator it(subDir, QDirIterator::Subdirectories);
        while(it.hasNext())
        {
            it.next();

            bool mesh = options.iMeshesOptimizationLevel >=1 && it.fileName().endsWith(".nif", Qt::CaseInsensitive);
            bool textureDDS = options.iTexturesOptimizationLevel >=2 && it.fileName().endsWith(".dds", Qt::CaseInsensitive);
            bool textureTGA = options.iTexturesOptimizationLevel >=1 && it.fileName().endsWith(".tga", Qt::CaseInsensitive);
            bool animation = options.bAnimationsOptimization && it.fileName().endsWith(".hkx", Qt::CaseInsensitive);

            bool bsa = options.bBsaExtract && it.fileName().endsWith(".bsa", Qt::CaseInsensitive);

            if(mesh || textureDDS || textureTGA || animation)
            {
                filesWeight += it.fileInfo().size();
                files << it.fileInfo();
            }

            else if(bsa)
            {
                filesWeight += it.fileInfo().size();
                files.insert(0, it.fileInfo());
            }
        }
    }
}

void Manager::parseArguments()
{
    QCommandLineParser parser;

    parser.addHelpOption();
    parser.addPositionalArgument("folder", "The file or the folder to process, surrounded with quotes.");
    parser.addPositionalArgument("game", "Currently, only 'sse' is supported");

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

    Manager::resetIni();

    QSettings settings("Cathedral Assets Optimizer.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "Cathedral Assets Optimizer.ini");

    QString path = QDir::cleanPath(parser.positionalArguments().at(0));
    QString mode = parser.positionalArguments().at(1);
    QString game = parser.positionalArguments().at(2);

    settings.setValue("SelectedPath", path);

    if(mode == "om")
        settings.setValue("iMode", 0);
    else if(mode == "sm")
        settings.setValue("iMode", 1);

    settings.setValue("Game", game);

    settings.setValue("bDryRun", parser.isSet("dr"));

    if(parser.isSet("l"))
        settings.setValue("iLogLevel", parser.value("l").toInt());

    if(parser.isSet("m"))
        settings.setValue("Meshes/iMeshesOptimizationLevel", parser.value("m").toInt());

    if(parser.isSet("t"))
        settings.setValue("Textures/iTexturesOptimizationLevel", parser.value("t").toInt());

    settings.setValue("Animations/bAnimationsOptimization", parser.isSet("a"));

    settings.beginGroup("BSA");
    settings.setValue("bBsaExtract", parser.isSet("be"));
    settings.setValue("bBsaCreate", parser.isSet("bc"));
    settings.setValue("bBsaDeleteBackup", parser.isSet("bd"));
    settings.endGroup();
}

bool Manager::checkSettings()
{
    if(!QDir(userPath).exists() || userPath.size() < 5)
    {
        QTextStream(stderr) << "\nError. This path does not exist or is shorter than 5 characters.";
        return false;
    }

    if (mode != 1 && mode != 0)
    {
        QTextStream(stderr) << "\nError. This mode does not exist.";
        return false;
    }

    if(options.game != SSE)
    {
        QTextStream(stderr) << "\nError. This game is not supported.";
        return false;
        //NOTE Will have to change this test once more games will be added.
    }

    if(options.iLogLevel < 0 || options.iLogLevel > 6)
    {
        QTextStream(stderr) << "\nError. This log level does not exist.";
        return false;
    }

    if(options.iMeshesOptimizationLevel < 0 || options.iMeshesOptimizationLevel > 3)
    {
        QTextStream(stderr) << "\nError. This log level does not exist.";
        return false;
    }

    if(options.iTexturesOptimizationLevel < 0 || options.iTexturesOptimizationLevel > 2)
    {
        QTextStream(stderr) << "\nError. This log level does not exist.";
        return false;
    }

    return true;
}

void Manager::resetIni()
{
    QString blankIni(QCoreApplication::applicationDirPath() + "/resources/defaultIni.ini");
    QString CathedralIni = "Cathedral Assets Optimizer.ini";
    QFile::remove(CathedralIni);
    QFile::copy(blankIni, CathedralIni);
}

void Manager::readIni()
{
    QSettings settings("Cathedral Assets Optimizer.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "Cathedral Assets Optimizer.ini");

    userPath = settings.value("SelectedPath").toString();
    QString game = settings.value("Game").toString();

    if(game == "sse")
        options.game = SSE;
    else
        return ; //TODO add error message

    QString iniMode = settings.value("iMode").toString();

    if(mode == 0)
        mode = singleMod;
    else if(mode == 1)
        mode = severalMods;
    else
        return ; //TODO add error

    options.bDryRun = settings.value("bDryRun").toBool();

    options.iLogLevel = settings.value("iLogLevel").toInt();

    settings.beginGroup("BSA");
    options.bBsaExtract = settings.value("bBsaExtract").toBool();
    options.bBsaCreate = settings.value("bBsaCreate").toBool();
    options.bBsaDeleteBackup = settings.value("bBsaDeleteBackup").toBool();
    options.bBsaOptimizeAssets = settings.value("bBsaOptimizeAssets").toBool();
    settings.endGroup();

    options.iMeshesOptimizationLevel = settings.value("Meshes/iMeshesOptimizationLevel").toInt();
    options.bMeshesHeadparts = settings.value("Meshes/bMeshesHeadparts").toBool();

    options.iTexturesOptimizationLevel = settings.value("Textures/iTexturesOptimizationLevel").toInt();

    options.bAnimationsOptimization = settings.value("Animations/bAnimationsOptimization").toBool();
}

bool Manager::checkRequirements()
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

int Manager::runOptimization()
{
    for(const auto& file : files)
    {
        completedFilesWeight += file.size();
        getOptimizer()->run(file.path());
    }

    return true;
}

MainOptimizer* Manager::getOptimizer()
{
    //TODO fix and optimize this ugly code
    while(true)
    {
        for(auto process : processes)
        {
            if(process->isFinished())
                return process;
        }
    }
}
