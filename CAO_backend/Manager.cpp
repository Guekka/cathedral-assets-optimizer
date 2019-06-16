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

    readIgnoredMods();

    QLogger::QLog_Info("MainOptimizer", "Listing files and directories...");

    listDirectories();
    listFiles();
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
            if(!subDir.contains("separator") && !ignoredMods.contains(subDir, Qt::CaseInsensitive)) //Separators are empty directories used by MO2
                modsToProcess << dir.filePath(subDir);
    }
}

void Manager::printProgress(const int& total, const QString& text = "Processing files")
{
    //'|' is used a separator by the frontend
    std::cout << "PROGRESS:|" << text.toStdString() << " - %v/%m - %p%|"  << numberCompletedFiles << '|' << total << std::endl;
}

void Manager::listFiles()
{
    numberFiles = 0;
    files.clear();
    animations.clear();
    BSAs.clear();

    for(auto& subDir : modsToProcess)
    {
        QDirIterator it(subDir, QDirIterator::Subdirectories);
        while(it.hasNext())
        {
            it.next();

            bool mesh = options.iMeshesOptimizationLevel >=1 && it.fileName().endsWith(".nif", Qt::CaseInsensitive);
            bool textureDDS = options.iTexturesOptimizationLevel >=2 && it.fileName().endsWith(".dds", Qt::CaseInsensitive);
            bool textureTGA = options.iTexturesOptimizationLevel >=1 && it.fileName().endsWith(".tga", Qt::CaseInsensitive);
            bool animation = options.bAnimationsOptimization && it.fileName().endsWith(".hkx", Qt::CaseInsensitive);

            bool bsa = options.bBsaExtract && it.fileName().endsWith(CAO_BSA_EXTENSION, Qt::CaseInsensitive);

            auto addToList = [&](QStringList& list){
                ++numberFiles;
                list << it.filePath();
            };

            if(mesh || textureDDS || textureTGA)
                addToList(files);
            else if(animation)
                addToList(animations);
            else if(bsa)
                addToList(BSAs);
        }
    }
}

void Manager::parseArguments()
{
    QCommandLineParser parser;

    parser.addHelpOption();
    parser.addPositionalArgument("folder", "The folder to process, surrounded with quotes.");
    parser.addPositionalArgument("mode", "Either om (one mod) or sm (several mods)");
    parser.addPositionalArgument("game", "Currently, only 'SSE', 'TES5', 'FO4' and 'Custom' are supported");

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
    else if(game == "FO4")
        CAO_SET_CURRENT_GAME(FO4)
    else if(game == "Custom")
        CAO_SET_CURRENT_GAME(Custom)
    else
        throw std::runtime_error("Cannot set game. Game:\"" + game.toStdString() + "\" does not exist");
}

void Manager::readIgnoredMods()
{
    QFile ignoredModsFile(CAO_RESOURCES_PATH + "ignoredMods.txt");
    if(ignoredModsFile.open(QIODevice::ReadOnly))
    {
        QTextStream ts(&ignoredModsFile);
        while (!ts.atEnd())
        {
            QString readLine = ts.readLine();
            if(readLine.left(1) != "#" && !readLine.isEmpty())
                ignoredMods << readLine;
        }
    }
    else
        QLogger::QLog_Warning("MainOptimizer", tr("ignoredMods.txt not found. All mods will be processed, including tools such as Nemesis or Bodyslide studio."));
}

void Manager::runOptimization()
{
    QLogger::QLog_Info("MainOptimizer", "Beginning...");

    MainOptimizer optimizer(options);

    //Reading headparts. Used for meshes optimization
    optimizer.addHeadparts(userPath, mode == severalMods);

    //Extracting BSAs
    for(const auto& file : BSAs)
    {
        optimizer.process(file);
        ++numberCompletedFiles;
        printProgress(BSAs.size(), "Extracting BSAs");
    }

    //Listing new extracted files
    listFiles();

    //Processing animations separately since they cannot be processed in a multithreaded way
    printProgress(numberFiles, "Processing animations");
    for(const auto& file : animations)
    {
        optimizer.process(file);
        ++numberCompletedFiles;
        if(numberCompletedFiles % 10 == 0)
            printProgress(numberFiles);
    }


    //Processing other files
    for(const auto& file : files)
    {
        //Processing files in several threads
        QVector<QFuture <void>> futures;
            futures << QtConcurrent::run([&]()
            {
                optimizer.process(file);
            });

        //Waiting for the processes to be completed
        for(auto& future : futures)
        {
            future.waitForFinished();
            ++numberCompletedFiles;
            if(numberCompletedFiles % 10 == 0)
                printProgress(numberFiles);
        }
    }

    numberCompletedFiles = 0;
    printProgress(modsToProcess.size(), "Packing BSAs");

    //Packing BSAs
    if(options.bBsaCreate)
        for(const auto& folder : modsToProcess)
        {
            optimizer.packBsa(folder);
            ++numberCompletedFiles;
            printProgress(modsToProcess.size(), "Packing BSAs");
        }

    FilesystemOperations::deleteEmptyDirectories(userPath);

    QLogger::QLog_Info("MainOptimizer", "\n\n\nProcess completed");

    return;
}

