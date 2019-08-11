/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Manager.h"

Manager::Manager(OptionsCAO &opt)
    : _options(opt)

{
#ifndef GUI
    //Setting game
    setGame();

    //Reading arguments
    if (QCoreApplication::arguments().count() >= 2)
        options.parseArguments(QCoreApplication::arguments());
#endif
    //Preparing logging
    initCustomLogger(Profiles::logPath(), _options.bDebugLog);

    PLOG_VERBOSE << "Checking settings...";
    const QString error = _options.isValid();
    if (!error.isEmpty())
    {
        PLOG_FATAL << error;
        throw std::runtime_error("Options are not valid." + error.toStdString());
    }

    readIgnoredMods();

    PLOG_INFO << "Listing files and directories...";
    listDirectories();
    listFiles();
}

void Manager::listDirectories()
{
    _modsToProcess.clear();

    if (_options.mode == OptionsCAO::SingleMod)
        _modsToProcess << _options.userPath;

    else if (_options.mode == OptionsCAO::SeveralMods)
    {
        const QDir dir(_options.userPath);
        for (auto subDir : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
            if (!subDir.contains("separator")
                && !_ignoredMods.contains(subDir, Qt::CaseInsensitive)) //Separators are empty directories used by MO2
                _modsToProcess << dir.filePath(subDir);
    }
}

void Manager::printProgress(const int &total, const QString &text = "Processing files")
{
#ifndef GUI
    std::cout << "PROGRESS:|" << text.toStdString() << " - %v/%m - %p%|" << numberCompletedFiles << '|' << total
              << std::endl;
#else
    emit progressBarTextChanged(text, total, _numberCompletedFiles);
#endif
}

void Manager::listFiles()
{
    _numberFiles = 0;
    _files.clear();
    _animations.clear();
    BSAs.clear();

    for (auto &subDir : _modsToProcess)
    {
        QDirIterator it(subDir, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            it.next();

            const bool mesh = _options.iMeshesOptimizationLevel >= 1
                              && it.fileName().endsWith(".nif", Qt::CaseInsensitive);
            const bool textureDDS = it.fileName().endsWith(".dds", Qt::CaseInsensitive);
            const bool textureTGA = it.fileName().endsWith(".tga", Qt::CaseInsensitive);
            const bool animation = _options.bAnimationsOptimization
                                   && it.fileName().endsWith(".hkx", Qt::CaseInsensitive);

            const bool bsa = _options.bBsaExtract && it.fileName().endsWith(Profiles::bsaExtension(), Qt::CaseInsensitive);

            auto addToList = [&](QStringList &list) {
                ++_numberFiles;
                list << it.filePath();
            };

            if (mesh || textureDDS || textureTGA)
                addToList(_files);
            else if (animation)
                addToList(_animations);
            else if (bsa)
                addToList(BSAs);
        }
    }
}

void Manager::readIgnoredMods()
{
    QFile &&ignoredModsFile = Profiles::getFile("ignoredMods.txt");
    if (ignoredModsFile.open(QIODevice::ReadOnly))
    {
        QTextStream ts(&ignoredModsFile);
        while (!ts.atEnd())
        {
            QString readLine = ts.readLine();
            if (readLine.left(1) != "#" && !readLine.isEmpty())
                _ignoredMods << readLine;
        }
    }
    else
        PLOG_WARNING << "ignoredMods.txt not found. All mods will be processed, including tools such as Nemesis or "
                        "Bodyslide studio.";
}

void Manager::runOptimization()
{
    PLOG_DEBUG << "Game:" << Profiles::currentProfile();
    PLOG_INFO << "Beginning...";

    MainOptimizer optimizer(_options);

    //Reading headparts. Used for meshes optimization
    optimizer.addHeadparts(_options.userPath, _options.mode == OptionsCAO::SeveralMods);

    //Extracting BSAs
    for (const auto &file : BSAs)
    {
        optimizer.process(file);
        ++_numberCompletedFiles;
        printProgress(BSAs.size(), "Extracting BSAs");
    }

    //Listing new extracted files
    listFiles();

    //Processing animations separately since they cannot be processed in a multithreaded way
    printProgress(_numberFiles, "Processing animations");
    for (const auto &file : _animations)
    {
        optimizer.process(file);
        ++_numberCompletedFiles;
        if (_numberCompletedFiles % 10 == 0)
            printProgress(_numberFiles);
    }

    //Processing other files
    for (const auto &file : _files)
    {
        //Processing files in several threads
        QVector<QFuture<void>> futures;
        futures << QtConcurrent::run([&]() { optimizer.process(file); });

        //Waiting for the processes to be completed
        for (auto &future : futures)
        {
            future.waitForFinished();
            ++_numberCompletedFiles;
            if (_numberCompletedFiles % 10 == 0)
                printProgress(_numberFiles);
        }
    }

    _numberCompletedFiles = 0;
    printProgress(_modsToProcess.size(), "Packing BSAs");

    //Packing BSAs
    if (_options.bBsaCreate)
        for (const auto &folder : _modsToProcess)
        {
            optimizer.packBsa(folder);
            ++_numberCompletedFiles;
            printProgress(_modsToProcess.size(), "Packing BSAs");
        }

    FilesystemOperations::deleteEmptyDirectories(_options.userPath);

    PLOG_INFO << "\n\n\nProcess completed";
}
