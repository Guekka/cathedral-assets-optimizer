/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Manager.h"

Manager::Manager(OptionsCAO &opt)
    : options(opt)

{
#ifndef GUI
    //Setting game
    setGame();

    //Reading arguments
    if (QCoreApplication::arguments().count() >= 2)
        options.parseArguments(QCoreApplication::arguments());
#endif
    //Preparing logging
    initCustomLogger(Games::logPath(), options.bDebugLog);

    PLOG_VERBOSE << tr("Checking settings...");
    const QString error = options.isValid();
    if (!error.isEmpty())
    {
        PLOG_FATAL << error;
        throw std::runtime_error("Options are not valid." + error.toStdString());
    }

    readIgnoredMods();

    PLOG_INFO << tr("Listing files and directories...");
    listDirectories();
    listFiles();
}

void Manager::listDirectories()
{
    modsToProcess.clear();

    if (options.mode == OptionsCAO::singleMod)
        modsToProcess << options.userPath;

    else if (options.mode == OptionsCAO::severalMods)
    {
      const QDir dir(options.userPath);
        for (auto subDir : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
            if (!subDir.contains("separator")
                && !ignoredMods.contains(subDir, Qt::CaseInsensitive)) //Separators are empty directories used by MO2
                modsToProcess << dir.filePath(subDir);
    }
}

void Manager::printProgress(const int &total, const QString &text = "Processing files")
{
    //'|' is used a separator by the frontend
#ifndef GUI
    std::cout << "PROGRESS:|" << text.toStdString() << " - %v/%m - %p%|" << numberCompletedFiles << '|' << total
              << std::endl;
#else
    emit progressBarTextChanged(text, total, numberCompletedFiles);
#endif
}

void Manager::listFiles()
{
    numberFiles = 0;
    files.clear();
    animations.clear();
    BSAs.clear();

    for (auto &subDir : modsToProcess)
    {
        QDirIterator it(subDir, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            it.next();

            const bool mesh = options.iMeshesOptimizationLevel >= 1 && it.fileName().endsWith(".nif", Qt::CaseInsensitive);
            const bool textureDDS = it.fileName().endsWith(".dds", Qt::CaseInsensitive);
            const bool textureTGA = it.fileName().endsWith(".tga", Qt::CaseInsensitive);
            const bool animation = options.bAnimationsOptimization && it.fileName().endsWith(".hkx", Qt::CaseInsensitive);

            const bool bsa = options.bBsaExtract && it.fileName().endsWith(Games::bsaExtension(), Qt::CaseInsensitive);

            auto addToList = [&](QStringList &list) {
                ++numberFiles;
                list << it.filePath();
            };

            if (mesh || textureDDS || textureTGA)
                addToList(files);
            else if (animation)
                addToList(animations);
            else if (bsa)
                addToList(BSAs);
        }
    }
}

void Manager::readIgnoredMods()
{
    QFile ignoredModsFile(Games::resourcePath() + "ignoredMods.txt");
    if (ignoredModsFile.open(QIODevice::ReadOnly))
    {
        QTextStream ts(&ignoredModsFile);
        while (!ts.atEnd())
        {
            QString readLine = ts.readLine();
            if (readLine.left(1) != "#" && !readLine.isEmpty())
                ignoredMods << readLine;
        }
    }
    else
        PLOG_WARNING << tr("ignoredMods.txt not found. All mods will be processed, including tools such as Nemesis or "
                           "Bodyslide studio.");
}

void Manager::runOptimization()
{
    PLOG_DEBUG << "Game:" << Games::game();
    PLOG_INFO << "Beginning...";

    MainOptimizer optimizer(options);

    //Reading headparts. Used for meshes optimization
    optimizer.addHeadparts(options.userPath, options.mode == OptionsCAO::severalMods);

    //Extracting BSAs
    for (const auto &file : BSAs)
    {
        optimizer.process(file);
        ++numberCompletedFiles;
        printProgress(BSAs.size(), "Extracting BSAs");
    }

    //Listing new extracted files
    listFiles();

    //Processing animations separately since they cannot be processed in a multithreaded way
    printProgress(numberFiles, "Processing animations");
    for (const auto &file : animations)
    {
        optimizer.process(file);
        ++numberCompletedFiles;
        if (numberCompletedFiles % 10 == 0)
            printProgress(numberFiles);
    }

    //Processing other files
    for (const auto &file : files)
    {
        //Processing files in several threads
        QVector<QFuture<void>> futures;
        futures << QtConcurrent::run([&]() { optimizer.process(file); });

        //Waiting for the processes to be completed
        for (auto &future : futures)
        {
            future.waitForFinished();
            ++numberCompletedFiles;
            if (numberCompletedFiles % 10 == 0)
                printProgress(numberFiles);
        }
    }

    numberCompletedFiles = 0;
    printProgress(modsToProcess.size(), "Packing BSAs");

    //Packing BSAs
    if (options.bBsaCreate)
        for (const auto &folder : modsToProcess)
        {
            optimizer.packBsa(folder);
            ++numberCompletedFiles;
            printProgress(modsToProcess.size(), "Packing BSAs");
        }

    FilesystemOperations::deleteEmptyDirectories(options.userPath);

    PLOG_INFO << "\n\n\nProcess completed";

    return;
}
