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

    QLogger::QLog_Info("MainOptimizer", tr("Beginning..."));

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

    modpathDir.setPath(options.userPath + "/" + modDirs.at(0)); //In case it is ran from debug UI
    QLogger::QLog_Debug("MainOptimizer", "Modpath: " + modpathDir.path() + "\n");

    emit progressBarMaximumChanged((modDirs.size()*(options.bBsaExtract + 1 + options.bBsaCreate)));
}


int MainOptimizer::mainProcess() // Process the userPath according to all user options
{
    init();

    //Base logging

    QLogger::QLog_Info("MainOptimizer", tr("Beginning..."));

    //Requirements

    if(!checkRequirements())
    {
        QLogger::QLog_Fatal("MainOptimizer", tr("The setup function did not run as expected. Exiting the process."));
        emit end();
        return 1;
    }

    //Run dry mode instead of normal mode if dry run is choosed

    if(options.bDryRun)
    {
        //dryRun();
        emit end();
        return 0;
    }

    //This loop applies the selected optimizations to each mod specified in ModDirs

    for(int i=0; i < modDirs.size(); ++i)
    {
        modpathDir.setPath(options.userPath + "/" + modDirs.at(i));
        QDirIterator it(modpathDir, QDirIterator::Subdirectories);

        QLogger::QLog_Debug("MainOptimizer", "ModDirs size: " + QString::number(modDirs.size()) + "\nCurrent index: " + QString::number(i));
        QLogger::QLog_Info("MainOptimizer", tr("Current mod: ") + modpathDir.path());

        MeshesOptimizer meshesOptimizer;
        meshesOptimizer.list(modpathDir.path());

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

        while(it.hasNext())
        {
            emit progressBarBusy();

            if((options.bMeshesProcess && it.fileName().right(4).toLower() == ".nif"))
                meshesOptimizer.optimize(it.filePath());

            if(options.bTexturesFullOptimization && it.fileName().right(4).toLower() == ".dds")
                TexturesOptimizer::convertToBc7IfUncompressed(it.filePath());

            if(options.bTexturesNecessaryOptimization && it.fileName().right(4).toLower() == ".tga")
                TexturesOptimizer::convertTgaToDds(it.filePath());

            if(options.bAnimationsOptimization && it.fileName().right(4).toLower() == ".hkx")
                AnimationsOptimizer::optimize(it.filePath());
        }

        emit progressBarMaximumChanged((modDirs.size()*(options.bBsaExtract + 1 + options.bBsaCreate)));
        progressBarIncrease();

        if(options.bBsaCreate)
        {
            try
            {
                if (options.bBsaPackLooseFiles)
                    FilesystemOperations::splitAssets(modpathDir.path());

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
            }
            catch(const QString& e)
            {
                QLogger::QLog_Error("MainOptimizer", e);
                QLogger::QLog_Error("MainOptimizer", tr("BSA packing canceled."));
            }
            PluginsOperations::makeDummyPlugins(modpathDir.path());
            emit progressBarIncrease();
        }
    }

    //Deleting empty dirs

    system(qPrintable("cd /d \"" + options.userPath + R"(" && for /f "delims=" %d in ('dir /s /b /ad ^| sort /r') do rd "%d" >nul 2>nul)"));

    emit end();

    QLogger::QLog_Info("MainOptimizer", tr("Completed."));

    return 0;
}

/*
void MainOptimizer::dryRun() // Perform a dry run : list files without actually modifying them
{
    init();

    QLogger::QLog_Info("MainOptimizer", tr("Beginning the dry run..."));

    for(int i=0; i < modDirs.size(); ++i)
    {
        modpathDir.setPath(QDir::cleanPath(options.userPath + "/" + modDirs.at(i)));
        QDirIterator it(modpathDir, QDirIterator::Subdirectories);

        meshesList();

        QLogger::QLog_Info("MainOptimizer", tr("Current mod: ") + modpathDir.path());

        if(options.bBsaExtract)
        {
            QLogger::QLog_Info("MainOptimizer", tr("Extracting BSA..."));

            QDirIterator bsaIt(modpathDir);

            while(bsaIt.hasNext())
            {
                if(bsaIt.next().right(4) == ".bsa")
                {
                    QLogger::QLog_Note("MainOptimizer", tr("BSA found ! Extracting...(this may take a long time, do not force close the program): ") + bsaIt.fileName());
                    try
                    {
                        bsaExtract(bsaIt.filePath());
                    }
                    catch(const QString& e)
                    {
                        QLogger::QLog_Error("MainOptimizer", e);
                    }
                }
            }
            emit progressBarIncrease();
        }

        while(it.hasNext())
        {
            it.next();


            if(it.fileName().contains(".nif", Qt::CaseInsensitive))
            {
                if(options.bMeshesNecessaryOptimization && headparts.contains(it.filePath(), Qt::CaseInsensitive))
                    QLogger::QLog_Info("MainOptimizer", it.filePath() + tr(" would be optimized by Headparts meshes option"));

                else if(options.bMeshesMediumOptimization && otherMeshes.contains(it.filePath()))
                    QLogger::QLog_Info("MainOptimizer", it.filePath() + tr(" would be optimized lightly by the Other Meshes option"));

                else if(options.bMeshesNecessaryOptimization && crashingMeshes.contains(it.filePath(), Qt::CaseInsensitive))
                    QLogger::QLog_Info("MainOptimizer", it.filePath() + tr(" would be optimized in full by the Hard Crashing Meshes option."));

                else if(options.bMeshesFullOptimization)
                    QLogger::QLog_Info("MainOptimizer", it.filePath() + tr(" would be optimized lightly by the Other Meshes option"));
            }
            if((options.bTexturesFullOptimization) && it.fileName().contains(".dds", Qt::CaseInsensitive))
            {
                QProcess texDiag;
                QStringList texdiagArg;

                texdiagArg << "info" << it.filePath();

                texDiag.start(QCoreApplication::applicationDirPath() + "/resources/texdiag.exe", texdiagArg);
                texDiag.waitForFinished(-1);

                if(texDiag.readAllStandardOutput().contains("compressed = no"))
                    QLogger::QLog_Info("MainOptimizer", it.filePath() + tr(" would be optimized using BC7 compression."));
            }

            if((options.bTexturesNecessaryOptimization) && it.fileName().contains(".tga", Qt::CaseInsensitive))
                QLogger::QLog_Info("MainOptimizer", it.filePath() + tr(" would be converted to DDS"));
        }

        emit progressBarIncrease();


    }
    QLogger::QLog_Info("MainOptimizer", tr("Completed."));
}
*/

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


void MainOptimizer::loadSettings() //Loads settings from the ini file
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");

    options.mode = settings.value("options.mode").toInt();
    options.bDryRun = settings.value("DryRun").toBool();
    options.userPath = settings.value("SelectedPath").toString();

    logLevel = QLogger::intToLogLevel(settings.value("logLevel").toInt());

    options.bBsaExtract = settings.value("bBsaExtract").toBool();
    options.bBsaCreate = settings.value("bBsaCreate").toBool();
    options.bBsaPackLooseFiles = settings.value("bBsaPackLooseFiles").toBool();
    options.bBsaDeleteBackup = settings.value("bBsaDeleteBackup").toBool();
    options.bBsaSplitAssets = settings.value("bBsaSplitAssets").toBool();

    options.bMeshesProcess = settings.value(" meshesGroupBox").toBool();

    options.bTexturesNecessaryOptimization = settings.value("bTexturesNecessaryOptimization").toBool();
    options.bTexturesFullOptimization = settings.value("bTexturesFullOptimization").toBool();

    options.bAnimationsOptimization = settings.value("bAnimationsOptimization").toBool();
}

void MainOptimizer::setLogLevel(const QLogger::LogLevel &value)
{
    logLevel = value;
}
