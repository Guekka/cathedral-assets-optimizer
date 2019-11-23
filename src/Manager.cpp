/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Manager.hpp"
namespace CAO {
Manager::Manager(Settings &opt)
    : _settings(opt)

{
    init();
}

Manager::Manager(const QStringList &args)
{
    //Parsing args
    _settings.parseArguments(args);
    init();
}

void Manager::init()
{
    //Preparing logging
    initCustomLogger(Profiles::logPath(), _settings.getMandatoryValue<bool>(StandardSettings::bDebugLog));

    PLOG_VERBOSE << "Checking settings...";
    const QString error = _settings.isValid();
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

    const auto &userPath = _settings.getMandatoryValue<QString>(StandardSettings::sUserPath);

    if (_settings.getMandatoryValue<StandardSettings::OptimizationMode>(StandardSettings::eMode)
        == StandardSettings::SingleMod)
        _modsToProcess << userPath;

    else
    {
        const QDir dir(userPath);
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
#else
    emit progressBarTextChanged(text + "- %v/%m - %p%", total, _numberCompletedFiles);
#endif
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
            const QString &filename = it.next();

            if (it.fileInfo().size() == 0)
                continue;

            auto getBool = [this](const StandardSettings::StandardKey &key) {
                return _settings.getMandatoryValue<bool>(key);
            };

            const bool processMeshes = getBool(StandardSettings::bMeshesResave)
                                       || _settings.getMandatoryValue<uint>(StandardSettings::iMeshesOptimizationLevel);
            const bool mesh = filename.endsWith(".nif", Qt::CaseInsensitive) && processMeshes;

            const bool processTextures = getBool(StandardSettings::bTexturesMipmaps)
                                         || getBool(StandardSettings::bTexturesCompress)
                                         || getBool(StandardSettings::bTexturesNecessary)
                                         || getBool(StandardSettings::bTexturesResizeSize)
                                         || getBool(StandardSettings::bTexturesResizeRatio);

            const bool texture = (filename.endsWith(".dds", Qt::CaseInsensitive)
                                  || filename.endsWith(".tga", Qt::CaseInsensitive))
                                 && processTextures;

            const bool animation = getBool(StandardSettings::bAnimationsOptimization)
                                   && filename.endsWith(".hkx", Qt::CaseInsensitive);

            const bool bsa = getBool(StandardSettings::bBsaExtract)
                             && filename.endsWith(_settings.getMandatoryValue<QString>(AdvancedSettings::sBSAExtension),
                                                  Qt::CaseInsensitive);

            QStringList &list = bsa ? BSAs : _files;
            if (!mesh && !texture && !animation && !bsa)
                continue;

            list << filename;
            ++_numberFiles;
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
    PLOG_INFO << "Processing: " + _settings.getMandatoryValue<QString>(StandardSettings::sUserPath);
    PLOG_INFO << "Beginning...";

    MainOptimizer optimizer(_settings);

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
    for (const auto &file : _files)
    {
        optimizer.process(file);
        ++_numberCompletedFiles;
        if (_numberCompletedFiles % 10 == 0)
            printProgress(_numberFiles);
    }

    _numberCompletedFiles = 0;
    printProgress(_modsToProcess.size(), "Packing BSAs");

    //Packing BSAs
    if (_settings.getMandatoryValue<bool>(StandardSettings::bBsaCreate))
        for (const auto &folder : _modsToProcess)
        {
            optimizer.packBsa(folder);
            ++_numberCompletedFiles;
            printProgress(_modsToProcess.size(), "Packing BSAs - Folder:  " + QFileInfo(folder).fileName());
        }

    FilesystemOperations::deleteEmptyDirectories(_settings.getMandatoryValue<QString>(StandardSettings::sUserPath));
    PLOG_INFO << "Process completed<br><br><br>";
    emit end();
}
} // namespace CAO
