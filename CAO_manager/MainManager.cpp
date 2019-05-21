/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "MainManager.h"

MainManager::MainManager()
{
    if(QCoreApplication::arguments().count() < 2)
        readIni();
    else
    {
        parseArguments();
        readIni();
    }

    if(!checkSettings())
        throw std::runtime_error("Error while reading settings");

    listDirectories();
    listFiles();
}

void MainManager::readIni()
{
    QSettings settings("Cathedral Assets Optimizer.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "Cathedral Assets Optimizer.ini");

    options.mode = static_cast<OptimizationMode>(settings.value("iMode").toInt());
    options.userPath = settings.value("SelectedPath").toString();

    options.bDryRun = settings.value("bDryRun").toBool();

    options.iLogLevel = settings.value("iLogLevel").toInt();

    settings.beginGroup("BSA");
    options.bBsaExtract = settings.value("bBsaExtract").toBool();
    options.bBsaCreate = settings.value("bBsaCreate").toBool();
    options.bBsaDeleteBackup = settings.value("bBsaDeleteBackup").toBool();
    settings.endGroup();

    options.iMeshesOptimizationLevel = settings.value("Meshes/iMeshesOptimizationLevel").toInt();

    options.iTexturesOptimizationLevel = settings.value("Textures/iTexturesOptimizationLevel").toInt();

    options.bAnimationsOptimization = settings.value("Animations/bAnimationsOptimization").toBool();
}

void MainManager::listDirectories()
{
    options.modsToProcess.clear();

    if(options.mode == OptimizationMode::singleMod)
        options.modsToProcess << options.userPath;

    else if(options.mode == OptimizationMode::severalMods)
    {
        QDir dir(options.userPath);
        for(auto subDir : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
        {
            if(!subDir.contains("separator")) //Separators are empty directories used by MO
                options.modsToProcess << dir.filePath(subDir);
        }
    }
}

void MainManager::printProgress()
{
    int progress = (completedFilesWeight / filesWeight) * 100;
    QTextStream ts(stdout);

    ts << progress;
}

void MainManager::listFiles()
{
    filesWeight = 0;
    files.clear();

    for(auto subDir : options.modsToProcess)
    {
        QDirIterator it(subDir, QDirIterator::Subdirectories);
        while(it.hasNext())
        {
            it.next();

            bool mesh = options.iMeshesOptimizationLevel >=1 && it.fileName().endsWith(".nif", Qt::CaseInsensitive);
            bool textureDDS = options.iTexturesOptimizationLevel >=2 && it.fileName().endsWith(".dds", Qt::CaseInsensitive);
            bool textureTGA = options.iTexturesOptimizationLevel >=1 && it.fileName().endsWith(".tga", Qt::CaseInsensitive);
            bool animation = options.bAnimationsOptimization && it.fileName().endsWith(".hkx", Qt::CaseInsensitive);

            if(mesh || textureDDS || textureTGA || animation)
            {
                filesWeight += it.fileInfo().size();
                files << it.fileInfo();
            }
        }
    }
}

void MainManager::parseArguments()
{
    QCommandLineParser parser;

    parser.addHelpOption();
    parser.addPositionalArgument("folder", "The folder to process, surrounded with quotes");
    parser.addPositionalArgument("mode", "Either om (one mod) or sm (several mods)");

    parser.addOptions({
                          {"dr", "Enables dry run"},
                          {"l", "Log level: from 0 (maximum) to 6", "value", "0"},
                          {"m", "Mesh processing level: 0 (default) to disable optimization, 1 for necessary optimization, 2 for medium optimization, 3 for full optimization.", "value", "0"},
                          {"t", "Texture processing level: 0 (default) to disable optimization, 1 for necessary optimization, 2 for full optimization.", "value", "0"},
                          {"a", "Enables animations processing"},
                          {"be", "Enables bsa extraction."},
                          {"bc", "Enables bsa creation."},
                          {"bd", "Enables deletion of bsa backups."},
                      });

    parser.process(QCoreApplication::arguments());
    MainManager::resetIni();


    QSettings settings("Cathedral Assets Optimizer.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "Cathedral Assets Optimizer.ini");

    QString path = QDir::cleanPath(parser.positionalArguments().at(0));
    QString mode = parser.positionalArguments().at(1);

    settings.setValue("SelectedPath", path);

    if(mode == "om")
        settings.setValue("iMode", 0);
    else if(mode == "sm")
        settings.setValue("iMode", 1);

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

bool MainManager::checkSettings()
{
    if(!QDir(options.userPath).exists() || options.userPath.size() < 5)
    {
        QTextStream(stderr) << "\nError. This path does not exist or is shorter than 5 characters.";
        return false;
    }

    if (options.mode != 1 && options.mode != 0)
    {
        QTextStream(stderr) << "\nError. This mode does not exist.";
        return false;
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

void MainManager::resetIni()
{
    QString blankIni(QCoreApplication::applicationDirPath() + "/resources/defaultIni.ini");
    QString CathedralIni = "Cathedral Assets Optimizer.ini";
    QFile::remove(CathedralIni);
    QFile::copy(blankIni, CathedralIni);
}

QString MainManager::getArguments(const QString &filePath)
{
    //TODO
    //NOTE What if the user want to pack BSAs?
    //NOTE Do not forget to handle headparts
}
