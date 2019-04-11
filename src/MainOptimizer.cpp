#include "MainOptimizer.h"

MainOptimizer::MainOptimizer() : logManager(QLogger::QLoggerManager::getInstance()), logLevel(QLogger::LogLevel::Info) {}


void MainOptimizer::init() //Some necessary operations before running
{     
    loadSettings();
    //Preparing logging
    QLogger::QLoggerManager *logManager = QLogger::QLoggerManager::getInstance();
    logManager->addDestination("log.html", QStringList() << "MainWindow" << "MainOptimizer" << "AnimationsOptimizer"
                               << "BsaOptimizer" << "FilesystemOperations" << "MeshesOptimizer" << "PluginsOperations"
                               << "TexturesOptimizer", logLevel);

    logManager->setLogLevelForAllWriters(logLevel);

    //Note log level is required for dry run

    if(options.bDryRun && QLogger::logLevelToInt(logLevel) > 2)
        logManager->setLogLevelForAllWriters(QLogger::LogLevel::Note);

    //Disabling BSA process if Skyrim folder is choosed

    if(options.userPath == FilesystemOperations::findSkyrimDirectory() + "/data" && (options.bBsaExtract || options.bBsaCreate))
    {
        QLogger::QLog_Error("MainOptimizer", tr("You are currently in the Skyrim directory. BSA won't be processed"));
        options.bBsaExtract = false;
        options.bBsaCreate = false;
        options.bBsaPackLooseFiles = false;
        options.bBsaDeleteBackup = false;
        options.bBsaSplitAssets = false;
    }

    fillModsLists();

    emit progressBarMaximumChanged((modDirs.size()*(options.bBsaExtract + 1 + options.bBsaCreate)));
}


int MainOptimizer::mainProcess() // Process the userPath according to all user options
{
    init();

    //Base logging

    QLogger::QLog_Info("MainOptimizer", tr("Beginning..."));
    QFile iniFile(QCoreApplication::applicationDirPath() + "/Cathedral Assets Optimizer.ini");
    iniFile.open(QIODevice::ReadOnly);
    QLogger::QLog_Debug("MainOptimizer", iniFile.readAll());
    iniFile.close();

    //Requirements

    if(!checkRequirements())
    {
        QLogger::QLog_Fatal("MainOptimizer", tr("The setup function did not run as expected. Exiting the process."));
        emit end();
        return 1;
    }

    //This loop applies the selected optimizations to each mod specified in ModDirs

    QString modpathDir;

    for(int i=0; i < modDirs.size(); ++i)
    {
        modpathDir = QDir::cleanPath(options.userPath + "/" + modDirs.at(i));

        QLogger::QLog_Debug("MainOptimizer", "ModDirs size: " + QString::number(modDirs.size()) + "\nCurrent index: " + QString::number(i));
        QLogger::QLog_Info("MainOptimizer", tr("Current mod: ") + modpathDir);

        BsaOptimizer bsaOptimizer;

        if(options.bBsaExtract)
        {
            QLogger::QLog_Info("MainOptimizer", tr("Extracting BSA..."));

            QDirIterator bsaIt(modpathDir);

            while(bsaIt.hasNext())
            {
                if(bsaIt.next().right(4) == ".bsa")
                {
                    QLogger::QLog_Note("MainOptimizer", tr("BSA found ! Extracting...(this may take a long time, do not force close the program): ") + bsaIt.fileName());
                    bsaOptimizer.bsaExtract(bsaIt.filePath(), !options.bBsaDeleteBackup);
                }
            }
            emit progressBarIncrease();
        }

        QLogger::QLog_Info("MainOptimizer", tr("Optimizing animations, textures and meshes..."));

        emit progressBarBusy();

        if(options.bDryRun)
            dryOptimizeAssets(modpathDir);
        else
            optimizeAssets(modpathDir);

        emit progressBarMaximumChanged((modDirs.size()*(options.bBsaExtract + 1 + options.bBsaCreate)));
        emit progressBarIncrease();

        FilesystemOperations fsOperations;

        if (options.bBsaPackLooseFiles || options.bBsaSplitAssets)
            fsOperations.prepareBsas(modpathDir, options.bBsaSplitAssets, options.bBsaMergeLoose);

        if(options.bBsaCreate)
        {
            QLogger::QLog_Info("MainOptimizer", tr("Creating BSAs..."));

            QDirIterator bsaIt(modpathDir);
            while(bsaIt.hasNext())
            {
                bsaIt.next();
                if(bsaIt.fileName().right(13) == "bsa.extracted")
                {
                    QLogger::QLog_Trace("MainOptimizer", "bsa folder found: " + bsaIt.fileName());
                    bsaOptimizer.bsaCreate(bsaIt.filePath());
                }
            }

            PluginsOperations::makeDummyPlugins(modpathDir);
            emit progressBarIncrease();
        }
    }

    //Deleting empty dirs

    system(qPrintable("cd /d \"" + options.userPath + R"(" && for /f "delims=" %d in ('dir /s /b /ad ^| sort /r') do rd "%d" >nul 2>nul)"));

    emit end();

    QLogger::QLog_Info("MainOptimizer", tr("Assets optimization completed.") + "\n\nEND OF LOG\n\n");

    return 0;
}

void MainOptimizer::fillModsLists()    //Adding all dirs to process to modDirs
{
    if(options.mode == 1) //Several mods mode
    {
        QDir dir(options.userPath);
        dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
        modDirs = dir.entryList();
    }

    if(options.mode == 0) //One mod mode
        modDirs << ""; //if modDirs is empty, the loop won't be run
}


bool MainOptimizer::checkRequirements()  //Checking if all the requirements are in the resources folder
{
    QStringList requirements {"bsarch.exe", "NifScan.exe", "nifopt.exe", "texconv.exe", "texdiag.exe", "ListHeadParts.exe"};
    QFile havokFile(FilesystemOperations::findSkyrimDirectory() + "/Tools/HavokBehaviorPostProcess/HavokBehaviorPostProcess.exe");

    if(havokFile.exists() && !QFile(QCoreApplication::applicationDirPath() + "/resources/HavokBehaviorPostProcess.exe").exists())
        havokFile.copy(QCoreApplication::applicationDirPath() + "/resources/HavokBehaviorPostProcess.exe");

    else if(!havokFile.exists() && !QFile(QCoreApplication::applicationDirPath() + "/resources/HavokBehaviorPostProcess.exe").exists())
        QLogger::QLog_Warning("MainOptimizer", tr("Havok Tool not found. Are you sure the Creation Kit is installed ? You can also put HavokBehaviorPostProcess.exe in the resources folder"));

    if(options.bAnimationsOptimization)
        requirements << "HavokBehaviorPostProcess.exe";

    for (int i = 0; i < requirements.size(); ++i)
    {
        QFile file(QCoreApplication::applicationDirPath() + "/resources/" + requirements.at(i));
        if(!file.exists())
        {
            QLogger::QLog_Error("MainOptimizer", requirements.at(i) + tr(" not found. Cancelling."));
            return false;
        }
    }
    return true;
}


void MainOptimizer::optimizeAssets(const QString& folderPath)
{
    QDirIterator it(folderPath, QDirIterator::Subdirectories);

    MeshesOptimizer meshesOptimizer;
    meshesOptimizer.list(folderPath);

    while(it.hasNext())
    {
        it.next();

        if((options.bMeshesProcess && it.fileName().right(4).toLower() == ".nif"))
            meshesOptimizer.optimize(it.filePath());

        if(options.bTexturesFullOptimization && it.fileName().right(4).toLower() == ".dds")
            TexturesOptimizer::convertToBc7IfUncompressed(it.filePath());

        if(options.bTexturesNecessaryOptimization && it.fileName().right(4).toLower() == ".tga")
            TexturesOptimizer::convertTgaToDds(it.filePath());

        if(options.bAnimationsOptimization && it.fileName().right(4).toLower() == ".hkx")
            AnimationsOptimizer::optimize(it.filePath());
    }
}


void MainOptimizer::dryOptimizeAssets(const QString& folderPath)
{
    QDirIterator it(folderPath, QDirIterator::Subdirectories);

    MeshesOptimizer meshesOptimizer;
    meshesOptimizer.list(folderPath);

    logManager->setLogLevelForAllWriters(QLogger::LogLevel::Note);

    while(it.hasNext())
    {
        it.next();

        if((options.bMeshesProcess && it.fileName().right(4).toLower() == ".nif"))
            meshesOptimizer.dryOptimize(it.filePath());

        if(options.bTexturesFullOptimization && it.fileName().right(4).toLower() == ".dds")
        {
            if(TexturesOptimizer::isCompressed(it.filePath()))
                QLogger::QLog_Note("MainOptimizer", it.filePath() + tr(" would be compressed to BC7"));
        }

        if(options.bTexturesNecessaryOptimization && it.fileName().right(4).toLower() == ".tga")
            QLogger::QLog_Note("MainOptimizer", it.filePath() + tr(" would be converted to DDS"));

        if(options.bAnimationsOptimization && it.fileName().right(4).toLower() == ".hkx")
            QLogger::QLog_Note("MainOptimizer", it.filePath() + tr(" would be ported to SSE"));
    }

    logManager->setLogLevelForAllWriters(logLevel);
}

void MainOptimizer::loadSettings() //Loads settings from the ini file
{
    QSettings settings("Cathedral Assets Optimizer.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "Cathedral Assets Optimizer.ini");

    options.mode = settings.value("mode").toInt();
    options.bDryRun = settings.value("DryRun").toBool();
    options.userPath = settings.value("SelectedPath").toString();

    logLevel = QLogger::intToLogLevel(settings.value("logLevel").toInt());

    options.bBsaExtract = settings.value("bBsaExtract").toBool();
    options.bBsaCreate = settings.value("bBsaCreate").toBool();
    options.bBsaPackLooseFiles = settings.value("bBsaPackLooseFiles").toBool();
    options.bBsaDeleteBackup = settings.value("bBsaDeleteBackup").toBool();
    options.bBsaSplitAssets = settings.value("bBsaSplitAssets").toBool();
    options.bBsaMergeLoose = settings.value("bBsaMergeLoose").toBool();

    options.bMeshesProcess = settings.value("meshesGroupBox").toBool();

    options.bTexturesNecessaryOptimization = settings.value("bTexturesNecessaryOptimization").toBool();
    options.bTexturesFullOptimization = settings.value("bTexturesFullOptimization").toBool();

    options.bAnimationsOptimization = settings.value("bAnimationsOptimization").toBool();
}


