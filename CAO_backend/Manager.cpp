/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Manager.h"

Manager::Manager()
{
    //Setting game
    setGame();

    //Preparing logging
    QLogger::QLoggerManager *logManager = QLogger::QLoggerManager::getInstance();
    logManager->addDestination(CAO_LOG_PATH, QStringList() << "MainWindow" << "MainOptimizer" << "AnimationsOptimizer"
                               << "BsaOptimizer" << "FilesystemOperations" << "MeshesOptimizer" << "PluginsOperations"
                               << "TexturesOptimizer", static_cast<QLogger::LogLevel>(options.iLogLevel));

    //INI
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, CAO_INI_PATH);
    settings = new QSettings(CAO_INI_PATH, QSettings::IniFormat, this);

    //Reading arguments

    if(QCoreApplication::arguments().count() >= 2)
        parseArguments();

    QLogger::QLog_Trace("MainOptimizer", "Reading INI...");

    readIni();

    QLogger::QLog_Trace("MainOptimizer", "Checking requirements...");

    if(!checkRequirements())
    {
        QLogger::QLog_Fatal("MainOptimizer", tr("The requirements were not found. Exiting the process."));
        throw std::runtime_error("Error while checking the requirements");
    }

    QLogger::QLog_Trace("MainOptimizer", "Checking settings...");

    if(!checkSettings())
    {
        QLogger::QLog_Fatal("MainOptimizer", tr("The settings were not read correctly. Exiting the process."));
        throw std::runtime_error("Error while reading settings");
    }

    QLogger::QLog_Info("MainOptimizer", "Listing files and directories...");

    listDirectories();
    listFiles(true);
}

void Manager::listDirectories()
{
    modsToProcess.clear();

    if(mode == OptimizationMode::singleMod)
    {
        FilePathSize folder;
        folder.filepath = userPath;
        modsToProcess << folder;
    }

    else if(mode == OptimizationMode::severalMods)
    {
        QDir dir(userPath);
        for(auto subDir : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
        {
            FilePathSize folder;
            folder.filepath = dir.filePath(subDir);
            if(!subDir.contains("separator")) //Separators are empty directories used by MO2
                modsToProcess << folder;
        }
    }
}

void Manager::printProgress()
{
    if(filesWeight == 0)
        return;
    double progress = static_cast<double> (completedFilesWeight * 100.0) / static_cast<double> (filesWeight * 100.0);
    std::cout << progress << std::endl;
}

void Manager::listFiles(const bool& calculateFileWeight)
{
    if(calculateFileWeight)
        filesWeight = 0;
    files.clear();
    animations.clear();
    BSAs.clear();

    for(auto& subDir : modsToProcess)
    {
        QDirIterator it(subDir.filepath, QDirIterator::Subdirectories);
        subDir.size = 0;
        while(it.hasNext())
        {
            it.next();

            bool mesh = options.iMeshesOptimizationLevel >=1 && it.fileName().endsWith(".nif", Qt::CaseInsensitive);
            bool textureDDS = options.iTexturesOptimizationLevel >=2 && it.fileName().endsWith(".dds", Qt::CaseInsensitive);
            bool textureTGA = options.iTexturesOptimizationLevel >=1 && it.fileName().endsWith(".tga", Qt::CaseInsensitive);
            bool animation = options.bAnimationsOptimization && it.fileName().endsWith(".hkx", Qt::CaseInsensitive);

            bool bsa = options.bBsaExtract && it.fileName().endsWith(CAO_BSA_EXTENSION, Qt::CaseInsensitive);

            auto addToList = [&](QVector<FilePathSize>& list)
            {
                FilePathSize file;
                file.filepath = it.filePath();
                file.size = static_cast<uint>(it.fileInfo().size());
                if(calculateFileWeight)
                    filesWeight += it.fileInfo().size();
                subDir.size += file.size;
                list << file;
            };

            if(mesh || textureDDS || textureTGA)
                addToList(files);
            else if(animation)
                addToList(animations);
            else if(bsa)
                addToList(BSAs);
        }
        if(options.bBsaCreate && calculateFileWeight)
            filesWeight += subDir.size;
    }
}

void Manager::parseArguments()
{
    QCommandLineParser parser;

    parser.addHelpOption();
    parser.addPositionalArgument("folder", "The folder to process, surrounded with quotes.");
    parser.addPositionalArgument("mode", "Either om (one mod) or sm (several mods)");
    parser.addPositionalArgument("game", "Currently, only 'SSE' and 'TES5' are supported");

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

    QString path = QDir::cleanPath(parser.positionalArguments().at(0));
    QString mode = parser.positionalArguments().at(1);
    QString game = parser.positionalArguments().at(2);

    settings->setValue("SelectedPath", path);

    if(mode == "om")
        settings->setValue("iMode", 0);
    else if(mode == "sm")
        settings->setValue("iMode", 1);

    settings->setValue("Game", game);

    settings->setValue("bDryRun", parser.isSet("dr"));

    if(parser.isSet("l"))
        settings->setValue("iLogLevel", parser.value("l").toInt());

    if(parser.isSet("m"))
        settings->setValue("Meshes/iMeshesOptimizationLevel", parser.value("m").toInt());

    if(parser.isSet("t"))
        settings->setValue("Textures/iTexturesOptimizationLevel", parser.value("t").toInt());

    settings->setValue("Animations/bAnimationsOptimization", parser.isSet("a"));

    settings->beginGroup("BSA");
    settings->setValue("bBsaExtract", parser.isSet("be"));
    settings->setValue("bBsaCreate", parser.isSet("bc"));
    settings->setValue("bBsaDeleteBackup", parser.isSet("bd"));
    settings->endGroup();
}

bool Manager::checkSettings()
{
    if(!QDir(userPath).exists() || userPath.size() < 5)
    {
        QLogger::QLog_Error("MainOptimizer", "\nError. This path does not exist or is shorter than 5 characters. Path:'" + userPath + "'");
        return false;
    }

    if (mode != singleMod && mode != severalMods)
    {
        QLogger::QLog_Error("MainOptimizer", "\nError. This mode does not exist.");
        return false;
    }

    if(options.iLogLevel < 0 || options.iLogLevel > 6)
    {
        QLogger::QLog_Error("MainOptimizer", "\nError. This log level does not exist. Log level: " + QString::number(options.iLogLevel));
        return false;
    }

    if(options.iMeshesOptimizationLevel < 0 || options.iMeshesOptimizationLevel > 3)
    {
        QLogger::QLog_Error("MainOptimizer", "\nError. This meshes optimization level does not exist. Level: " + QString::number(options.iMeshesOptimizationLevel));
        return false;
    }

    if(options.iTexturesOptimizationLevel < 0 || options.iTexturesOptimizationLevel > 2)
    {
        QLogger::QLog_Error("MainOptimizer", "\nError. This textures optimization level does not exist. Level: " + QString::number(options.iTexturesOptimizationLevel));
        return false;
    }

    return true;
}

void Manager::resetIni()
{
    QString blankIni("resources/defaultIni.ini");
    QFile::remove(CAO_INI_PATH);
    QFile::copy(blankIni, CAO_INI_PATH);
}

void Manager::readIni()
{
    QFile ini(CAO_INI_PATH);
    ini.open(QFile::ReadOnly);
    QLogger::QLog_Debug("MainOptimizer", "Ini content:\n" + ini.readAll() );
    ini.close();

    userPath = settings->value("SelectedPath").toString();

    int iniMode = settings->value("iMode").toInt();

    if(iniMode == 0)
        mode = singleMod;
    else if(iniMode == 1)
        mode = severalMods;

    options.bDryRun = settings->value("bDryRun").toBool();

    options.iLogLevel = settings->value("iLogLevel").toInt();

    settings->beginGroup("BSA");
    options.bBsaExtract = settings->value("bBsaExtract").toBool();
    options.bBsaCreate = settings->value("bBsaCreate").toBool();
    options.bBsaDeleteBackup = settings->value("bBsaDeleteBackup").toBool();
    options.bBsaOptimizeAssets = settings->value("bBsaOptimizeAssets").toBool();
    settings->endGroup();

    options.iMeshesOptimizationLevel = settings->value("Meshes/iMeshesOptimizationLevel").toInt();
    options.bMeshesHeadparts = settings->value("Meshes/bMeshesHeadparts").toBool();

    options.iTexturesOptimizationLevel = settings->value("Textures/iTexturesOptimizationLevel").toInt();

    options.bAnimationsOptimization = settings->value("Animations/bAnimationsOptimization").toBool();
}

bool Manager::checkRequirements()
{
    QStringList requirements {"texconv.exe", "texdiag.exe"};

    for (const auto& requirement : requirements)
    {
        QFile file("resources/" + requirement);

        if(!file.exists())
        {
            QLogger::QLog_Error("MainOptimizer", requirement + tr(" not found. Cancelling."));
            return false;
        }
    }
    return true;
}

void Manager::setGame()
{
    QSettings commonSettings("settings/common/config.ini", QSettings::IniFormat, this);
    QString game = commonSettings.value("Game").toString();

    QLogger::QLog_Debug("MainOptimizer", "Game: " + game);

    if(game == "SSE")
        CAO_SET_CURRENT_GAME(SSE)
    else if(game == "TES5")
        CAO_SET_CURRENT_GAME(TES5)
    else
        throw std::runtime_error("Cannot set game. Game:\"" + game.toStdString() + "\" does not exist");
}

void Manager::runOptimization()
{
    QLogger::QLog_Info("MainOptimizer", "Beginning...");

    MainOptimizer optimizer(options);

    //Reading headparts. Used for meshes optimization
    optimizer.addHeadparts(userPath, mode == severalMods);

    //Lambdas used to process files
    auto ProcessFile = [&](const FilePathSize& file, auto function, bool printProgress)
    {
        QLogger::QLog_Debug("MainOptimizer", "currentFile: " + file.filepath);
        (optimizer.*function)(file.filepath);
        completedFilesWeight += file.size;
        if(printProgress)
            this->printProgress();
    };


    auto ProcessFilesMultithreaded = [&](QVector<FilePathSize>& container, auto function)
    {
        //Processing files in several threads
        QVector<QFuture <void>> futures;
        for(const auto& file : container)
            futures << QtConcurrent::run([&]()
            {
                ProcessFile(file, function, false);
            });

        //Waiting for the processes to be completed
        for(auto& future : futures)
            future.waitForFinished();
    };

    //Extracting BSAs
    for(const auto& file : BSAs)
        ProcessFile(file, &MainOptimizer::process, true);

    //Listing new extracted files
    listFiles(false);

    //Processing animations separately since they cannot be processed in a multithreaded way
    for(const auto& file : animations)
        ProcessFile(file, &MainOptimizer::process, false);

    printProgress();

    //Processing other files
    ProcessFilesMultithreaded(files, &MainOptimizer::process);

    printProgress();

    //Packing BSAs
    if(options.bBsaCreate)
        for(const auto& info : modsToProcess)
            ProcessFile(info, &MainOptimizer::packBsa, true);

    FilesystemOperations::deleteEmptyDirectories(userPath);

    QLogger::QLog_Info("MainOptimizer", "\n\n\nProcess completed");

    return;
}

