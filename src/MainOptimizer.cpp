#include "MainOptimizer.h"

MainOptimizer::MainOptimizer() : logManager(QLoggerManager::getInstance()), logLevel(LogLevel::Info) {}


bool MainOptimizer::init() //Some necessary operations before running
{     
    logManager->addDestination("log.html", "Optimiser", logLevel);

    saveSettings();

    modDirs.clear();
    emit progressBarReset();

    //Preparing logging

    QLog_Info("Optimiser", tr("Beginning..."));
    printSettings();

    //Disabling BSA process if Skyrim folder is choosed

    if(options.userPath == findSkyrimDirectory() + "/data" && (options.bBsaExtract || options.bBsaCreate))
    {
        QLogger::QLog_Error("Optimiser", tr("You are currently in the Skyrim directory. BSA won't be processed"));
        options.bBsaExtract = false;
        options.bBsaCreate = false;
        options.bBsaPackLooseFiles = false;
        options.bBsaDeleteBackup = false;
        options.bBsaSplitAssets = false;
    }

    //Checking if all the requirements are in the resources folder

    QStringList requirements;
    QFile havokFile(findSkyrimDirectory() + "/Tools/HavokBehaviorPostProcess/HavokBehaviorPostProcess.exe");

    if(havokFile.exists() && !QFile(QCoreApplication::applicationDirPath() + "/resources/HavokBehaviorPostProcess.exe").exists())
        havokFile.copy(QCoreApplication::applicationDirPath() + "/resources/HavokBehaviorPostProcess.exe");

    else if(!havokFile.exists() && !QFile(QCoreApplication::applicationDirPath() + "/resources/HavokBehaviorPostProcess.exe").exists())
        QLogger::QLog_Warning("Optimiser", tr("Havok Tool not found. Are you sure the Creation Kit is installed ? You can also put HavokBehaviorPostProcess.exe in the resources folder"));

    requirements << "bsarch.exe" << "NifScan.exe" << "nifopt.exe" << "texconv.exe" << "texdiag.exe" << "ListHeadParts.exe";

    if(options.bAnimationsOptimization)
        requirements << "HavokBehaviorPostProcess.exe";

    for (int i = 0; i < requirements.size(); ++i)
    {
        QFile file(QCoreApplication::applicationDirPath() + "/resources/" + requirements.at(i));
        if(!file.exists())
        {
            QLogger::QLog_Error("Optimiser", requirements.at(i) + tr(" not found. Cancelling."));
            return false;
        }
    }

    //Adding all dirs to process to modDirs

    if(options.mode == 1) //Several mods mode
    {
        QDir dir(options.userPath);
        dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
        modDirs = dir.entryList();
    }

    if(options.mode == 0) //One mod mode
        modDirs << ""; //if modDirs is empty, the loop won't be run

    modpathDir.setPath(options.userPath + "/" + modDirs.at(0)); //In case it is ran from debug UI
    QLogger::QLog_Debug("Optimiser", "Modpath: " + modpathDir.path() + "\n");

    emit progressBarMaximumChanged((modDirs.size()*(options.bBsaExtract + 1 + options.bBsaCreate)));


    return true;
}


int MainOptimizer::mainProcess() // Process the userPath according to all user options
{
    if(!init())
    {
        emit end();
        QLog_Fatal("Optimiser", tr("The setup function did not run as expected. Exiting the process."));
        return 1;
    }

    if(options.bDryRun)
    {
        dryRun();
        emit end();
        return 2;
    }

    //This loop applies the selected optimizations to each mod specified in ModDirs

    for(int i=0; i < modDirs.size(); ++i)
    {
        modpathDir.setPath(options.userPath + "/" + modDirs.at(i));
        QDirIterator it(modpathDir, QDirIterator::Subdirectories);

        QLogger::QLog_Debug("Optimiser", "ModDirs size: " + QString::number(modDirs.size()) + "\nCurrent index: " + QString::number(i));
        QLog_Info("Optimiser", tr("Current mod: ") + modpathDir.path());

        meshesList();

        if(options.bBsaExtract)
        {
            QLog_Info("Optimiser", tr("Extracting BSA..."));

            QDirIterator bsaIt(modpathDir);

            while(bsaIt.hasNext())
            {
                if(bsaIt.next().right(4) == ".bsa")
                {
                    QLogger::QLog_Note("Optimiser", tr("BSA found ! Extracting...(this may take a long time, do not force close the program): ") + bsaIt.fileName());

                    try
                    {
                        bsaExtract(bsaIt.filePath());
                    }
                    catch(const QString& e)
                    {
                        QLogger::QLog_Error("Optimiser", e);
                    }
                }
            }
            emit progressBarIncrease();
        }

        QLog_Info("Optimiser", tr("Optimizing animations, textures and meshes..."));

        QString currentFile;

        while(it.hasNext())
        {
            emit progressBarBusy();

            currentFile = it.next();

            if((options.bMeshesNecessaryOptimization || options.bMeshesMediumOptimization || options.bMeshesFullOptimization) && it.fileName().right(4).toLower() == ".nif")
                meshesOptimize(currentFile);

            if((options.bTexturesFullOptimization) && it.fileName().right(4).toLower() == ".dds")
                texturesBc7Conversion(currentFile);

            if((options.bTexturesNecessaryOptimization) && it.fileName().right(4).toLower() == ".tga")
                texturesTgaToDds(currentFile);

            if(options.bAnimationsOptimization && it.fileName().right(4).toLower() == ".hkx")
                animationsOptimize(currentFile);
        }

        emit progressBarMaximumChanged((modDirs.size()*(options.bBsaExtract + 1 + options.bBsaCreate)));
        progressBarIncrease();

        if(options.bBsaCreate)
        {
            try
            {
                if (options.bBsaPackLooseFiles)
                    splitAssets();

                QDirIterator bsaIt(modpathDir);
                while(bsaIt.hasNext())
                {
                    bsaIt.next();
                    if(bsaIt.fileName().right(13) == "bsa.extracted")
                    {
                        QLogger::QLog_Trace("Optimiser", "bsa folder found: " + bsaIt.fileName());
                        bsaCreate(bsaIt.filePath());
                    }
                }
            }
            catch(const QString& e)
            {
                QLogger::QLog_Error("Optimiser", e);
                QLogger::QLog_Error("Optimiser", tr("BSA packing canceled."));
            }
            makeDummyPlugins(modpathDir.path());
            emit progressBarIncrease();
        }
    }

    //Deleting empty dirs

    system(qPrintable("cd /d \"" + options.userPath + R"(" && for /f "delims=" %d in ('dir /s /b /ad ^| sort /r') do rd "%d" >nul 2>nul)"));

    emit end();

    QLog_Info("Optimiser", tr("Completed."));

    return 0;
}


void MainOptimizer::dryRun() // Perform a dry run : list files without actually modifying them
{
    init();

    QLog_Info("Optimiser", tr("Beginning the dry run..."));

    for(int i=0; i < modDirs.size(); ++i)
    {
        modpathDir.setPath(QDir::cleanPath(options.userPath + "/" + modDirs.at(i)));
        QDirIterator it(modpathDir, QDirIterator::Subdirectories);

        meshesList();

        QLog_Info("Optimiser", tr("Current mod: ") + modpathDir.path());

        if(options.bBsaExtract)
        {
            QLog_Info("Optimiser", tr("Extracting BSA..."));

            QDirIterator bsaIt(modpathDir);

            while(bsaIt.hasNext())
            {
                if(bsaIt.next().right(4) == ".bsa")
                {
                    QLogger::QLog_Note("Optimiser", tr("BSA found ! Extracting...(this may take a long time, do not force close the program): ") + bsaIt.fileName());
                    try
                    {
                        bsaExtract(bsaIt.filePath());
                    }
                    catch(const QString& e)
                    {
                        QLogger::QLog_Error("Optimiser", e);
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
                    QLog_Info("Optimiser", it.filePath() + tr(" would be optimized by Headparts meshes option"));

                else if(options.bMeshesMediumOptimization && otherMeshes.contains(it.filePath()))
                    QLog_Info("Optimiser", it.filePath() + tr(" would be optimized lightly by the Other Meshes option"));

                else if(options.bMeshesNecessaryOptimization && crashingMeshes.contains(it.filePath(), Qt::CaseInsensitive))
                    QLog_Info("Optimiser", it.filePath() + tr(" would be optimized in full by the Hard Crashing Meshes option."));

                else if(options.bMeshesFullOptimization)
                    QLog_Info("Optimiser", it.filePath() + tr(" would be optimized lightly by the Other Meshes option"));
            }
            if((options.bTexturesFullOptimization) && it.fileName().contains(".dds", Qt::CaseInsensitive))
            {
                QProcess texDiag;
                QStringList texdiagArg;

                texdiagArg << "info" << it.filePath();

                texDiag.start(QCoreApplication::applicationDirPath() + "/resources/texdiag.exe", texdiagArg);
                texDiag.waitForFinished(-1);

                if(texDiag.readAllStandardOutput().contains("compressed = no"))
                    QLog_Info("Optimiser", it.filePath() + tr(" would be optimized using BC7 compression."));
            }

            if((options.bTexturesNecessaryOptimization) && it.fileName().contains(".tga", Qt::CaseInsensitive))
                QLog_Info("Optimiser", it.filePath() + tr(" would be converted to DDS"));
        }

        emit progressBarIncrease();


    }
    QLog_Info("Optimiser", tr("Completed."));
}




/* WORK IN PROGRESS */
/*void Optimiser::meshesTexturesCaseFix(const QString &filePath) //Unused. Work in progress. Same func as NIF Texcase Fixer
{
    QFile file(filePath);
    QString binaryData;
    QString texturePath;
    QStringList storedTextures;
    QVector <QStringRef> matches;

    QDirIterator textures(modpathDir, QDirIterator::Subdirectories);

    while (textures.hasNext())
    {
        if(textures.next().right(3).toLower() == "dds")
            storedTextures << modpathDir.relativeFilePath(textures.filePath());
    }

    file.open(QFile::ReadWrite);
    binaryData = QTextCodec::codecForMib(106)->toUnicode(file.read(999999));

    //qDebug() << filePath;

    if(binaryData.contains(".dds"))
    {
        matches = binaryData.splitRef(QRegularExpression(R"(?:[a-zA-Z]:(?:.*?))?textures(?:.*?)dds)"));
        for (const auto& match : matches)
        {
            for (const auto& tex : storedTextures)
            {
                if(match == tex)
                    break;

                if(match.toString().toLower() == tex.toLower())
                    binaryData.replace(match.toString().toUtf8(), tex.toUtf8());

                else if(match.endsWith(tex, Qt::CaseInsensitive))
                {
                    binaryData.replace(match.toString().toUtf8(), tex.toUtf8());
                }
            }
        }
    }

    file.close();
}*/
/* END WORK IN PROGRESS */







void MainOptimizer::saveSettings() //Saves settings to an ini file
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");


    settings.setValue("options.mode", options.mode);
    settings.setValue("DryRun", options.bDryRun);
    settings.setValue("SelectedPath", options.userPath);

    settings.setValue("logLevel", logLevelToInt(logLevel));

    settings.setValue("bBsaExtract", options.bBsaExtract);
    settings.setValue("bBsaCreate", options.bBsaCreate);
    settings.setValue("bBsaPackLooseFiles", options.bBsaPackLooseFiles);
    settings.setValue("bBsaDeleteBackup", options.bBsaDeleteBackup);
    settings.setValue("bBsaSplitAssets", options.bBsaSplitAssets);

    settings.setValue("bMeshesNecessaryOptimization", options.bMeshesNecessaryOptimization);
    settings.setValue("bMeshesMediumOptimization", options.bMeshesMediumOptimization);
    settings.setValue("bMeshesFullOptimization", options.bMeshesFullOptimization);

    settings.setValue("bTexturesNecessaryOptimization", options.bTexturesNecessaryOptimization);
    settings.setValue("bTexturesFullOptimization", options.bTexturesFullOptimization);

    settings.setValue("bAnimationsOptimization", options.bAnimationsOptimization);
}


void MainOptimizer::loadSettings() //Loads settings from the ini file
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");

    options.mode = settings.value("options.mode").toInt();
    options.bDryRun = settings.value("DryRun").toBool();
    options.userPath = settings.value("SelectedPath").toString();

    logLevel = intToLogLevel(settings.value("logLevel").toInt());

    options.bBsaExtract = settings.value("bBsaExtract").toBool();
    options.bBsaCreate = settings.value("bBsaCreate").toBool();
    options.bBsaPackLooseFiles = settings.value("bBsaPackLooseFiles").toBool();
    options.bBsaDeleteBackup = settings.value("bBsaDeleteBackup").toBool();
    options.bBsaSplitAssets = settings.value("bBsaSplitAssets").toBool();

    options.bMeshesNecessaryOptimization = settings.value("bMeshesNecessaryOptimization").toBool();
    options.bMeshesMediumOptimization = settings.value("bMeshesMediumOptimization").toBool();
    options.bMeshesFullOptimization = settings.value("bMeshesFullOptimization").toBool();

    options.bTexturesNecessaryOptimization = settings.value("bTexturesNecessaryOptimization").toBool();
    options.bTexturesFullOptimization = settings.value("bTexturesFullOptimization").toBool();

    options.bAnimationsOptimization = settings.value("bAnimationsOptimization").toBool();
}


void MainOptimizer::printSettings() //Will print settings into debug log
{
    QSettings settings("SSE Assets Optimiser.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "SSE Assets Optimiser.ini");

    QLogger::QLog_Debug("Optimiser", "mode: " + QString::number(options.mode));
    QLogger::QLog_Debug("Optimiser", "DryRun: " + QString::number(options.bDryRun));
    QLogger::QLog_Debug("Optimiser", "userPath: "+ options.userPath);

    QLogger::QLog_Debug("Optimiser", "LogLevel: " + QString::number(logLevelToInt(logLevel)));

    QLogger::QLog_Debug("Optimiser", "bBsaExtract: " + QString::number(options.bBsaExtract));
    QLogger::QLog_Debug("Optimiser", "bBsaCreate: " + QString::number(options.bBsaCreate));
    QLogger::QLog_Debug("Optimiser", "bBsaPackLooseFiles: " + QString::number(options.bBsaPackLooseFiles));
    QLogger::QLog_Debug("Optimiser", "bBsaDeleteBackup: " + QString::number(options.bBsaDeleteBackup));
    QLogger::QLog_Debug("Optimiser", "bBsaSplitAssets" + QString::number(options.bBsaSplitAssets));

    QLogger::QLog_Debug("Optimiser", "bMeshesNecessaryOptimization: " + QString::number(options.bMeshesNecessaryOptimization));
    QLogger::QLog_Debug("Optimiser", "bMeshesMediumOptimization: " + QString::number(options.bMeshesMediumOptimization));
    QLogger::QLog_Debug("Optimiser", "bMeshesFullOptimization: " + QString::number(options.bMeshesFullOptimization));

    QLogger::QLog_Debug("Optimiser", "bTexturesNecessaryOptimization: " + QString::number( options.bTexturesNecessaryOptimization));
    QLogger::QLog_Debug("Optimiser", "bTexturesFullOptimization: " + QString::number( options.bTexturesFullOptimization));

    QLogger::QLog_Debug("Optimiser", "bAnimationsOptimization: " + QString::number(options.bAnimationsOptimization));
}

void MainOptimizer::setLogLevel(const QLogger::LogLevel &value)
{
    logLevel = value;
}
