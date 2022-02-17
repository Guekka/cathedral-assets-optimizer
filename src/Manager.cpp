/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Manager.h"

Manager::Manager(const OptionsCAO& opt)
  : _options(opt)

{
    init();
}

void Manager::init()
{
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
    QTextStream(stdout) << "PROGRESS:|" << text << " - %v/%m - %p%|" << _numberCompletedFiles << '|' << total << endl;
#endif
#ifdef GUI
    emit progressBarTextChanged(text + "- %v/%m - %p%", total, _numberCompletedFiles);
#endif
}

void Manager::cancelProcess()
{
    _isCancelled = true;
}

void Manager::listFiles()
{
    _numberFiles = 0;
    _files.clear();
    BSAs.clear();

    for (auto &subDir : _modsToProcess)
    {
        QDirIterator it(subDir, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            it.next();

            const bool mesh = it.fileName().endsWith(".nif", Qt::CaseInsensitive);
            const bool textureDDS = it.fileName().endsWith(".dds", Qt::CaseInsensitive);
            const bool textureTGA = it.fileName().endsWith(".tga", Qt::CaseInsensitive);
            const bool animation = _options.bAnimationsOptimization
                                   && it.fileName().endsWith(".hkx", Qt::CaseInsensitive);

            const auto u8BsaExt = btu::bsa::Settings::get(Profiles::bsaGame()).extension;
            const auto asciiBsaExt = btu::common::as_ascii(u8BsaExt);
            const auto bsaExt = QString::fromUtf8(asciiBsaExt.data(),
                                                  static_cast<int>(asciiBsaExt.size()));

            const bool bsa = _options.bBsaExtract
                             && it.fileName().endsWith(bsaExt, Qt::CaseInsensitive);

            auto addToList = [&](QStringList &list) {
                ++_numberFiles;
                list << it.filePath();
            };

            if (mesh || textureDDS || textureTGA || animation)
                addToList(_files);
            else if (bsa)
                addToList(BSAs);
        }
    }
}

void Manager::readIgnoredMods()
{
    QFile &&ignoredModsFile = Profiles::getFile("ignoredMods.txt");
    _ignoredMods = FilesystemOperations::readFile(ignoredModsFile);

    if (_ignoredMods.isEmpty())
    {
        PLOG_WARNING << "ignoredMods.txt not found. All mods will be processed, including tools such as Nemesis or "
                        "Bodyslide studio.";
    }
}

void Manager::runOptimization()
{
    PLOG_DEBUG << "Game: " << Profiles::currentProfile();
    PLOG_INFO << "Processing: " + _options.userPath;
    PLOG_INFO << "Beginning...";

    MainOptimizer optimizer(_options);

    //Extracting BSAs
    for (const auto &file : BSAs) {
        if (_isCancelled)
            return;

        optimizer.process(file);
        ++_numberCompletedFiles;
        printProgress(BSAs.size(), "Extracting BSAs");
    }

    //Listing newly extracted files
    listFiles();

    //Using time in order to prevent printing progress too often
    QDateTime time1 = QDateTime::currentDateTime();
    QDateTime time2;
    for (const auto &file : _files)
    {
        optimizer.process(file);
        ++_numberCompletedFiles;
        if (_numberCompletedFiles % 10 == 0) {
            if (_isCancelled)
                return;

            time2 = QDateTime::currentDateTime();
            if (time2 > time1.addMSecs(3000))
            {
                printProgress(_numberFiles);
                time1 = time2;
            }
        }
    }

    _numberCompletedFiles = 0;
    printProgress(_modsToProcess.size(), "Packing BSAs");

    //Packing BSAs
    if (_options.bBsaCreate)
        for (const auto &folder : _modsToProcess)
        {
            if (_isCancelled)
                return;

            optimizer.packBsa(folder);
            ++_numberCompletedFiles;
            printProgress(_modsToProcess.size(),
                          "Packing BSAs - Folder:  " + QFileInfo(folder).fileName());
        }

    FilesystemOperations::deleteEmptyDirectories(_options.userPath);
    PLOG_INFO << "Process completed<br><br><br>";
    emit end();
}
