/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Manager.hpp"
namespace CAO {
Manager::Manager(const Profile &profile)
    : _profile(profile)

{
    init();
}

void Manager::init()
{
    //Preparing logging
    initCustomLogger(_profile.logPath(), _profile.getGeneralSettings().bDebugLog());

    PLOG_VERBOSE << "Checking settings...";
    const QString error = _profile.getGeneralSettings().isValid();
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

    const auto &settings = _profile.getGeneralSettings();
    if (settings.eMode() == SingleMod)
        _modsToProcess << settings.sUserPath();

    else
    {
        const QDir dir(settings.sUserPath());
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

            const auto &settings = _profile.getSettings(filename);

            const bool processMeshes = settings.bMeshesResave()
                                       || settings.iMeshesOptimizationLevel();
            const bool mesh = filename.endsWith(".nif", Qt::CaseInsensitive) && processMeshes;

            const bool processTextures = settings.bTexturesMipmaps() || settings.bTexturesCompress()
                                         || settings.bTexturesNecessary()
                                         || settings.bTexturesResizeSize()
                                         || settings.bTexturesResizeRatio();

            const bool texture = (filename.endsWith(".dds", Qt::CaseInsensitive)
                                  || filename.endsWith(".tga", Qt::CaseInsensitive))
                                 && processTextures;

            const bool animation = settings.bAnimationsOptimization()
                                   && filename.endsWith(".hkx", Qt::CaseInsensitive);

            const auto &generalSettings = _profile.getGeneralSettings();
            const bool bsa = generalSettings.bBsaExtract()
                             && filename.endsWith(generalSettings.sBSAExtension(),
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
    QFile &&ignoredModsFile = _profile.getFile("ignoredMods.txt");
    _ignoredMods = FilesystemOperations::readFile(ignoredModsFile);

    if (_ignoredMods.isEmpty())
    {
        PLOG_WARNING << "ignoredMods.txt not found. All mods will be processed, including tools such as Nemesis or "
                        "Bodyslide studio.";
    }
}

void Manager::runOptimization()
{
    PLOG_DEBUG << "Profile directory: " << Profiles().getCurrent().profileDirectory().path();
    PLOG_INFO << "Processing: " + _profile.getGeneralSettings().sUserPath();
    PLOG_INFO << "Beginning...";

    MainOptimizer optimizer(_profile);

    //Extracting BSAs
    for (const auto &file : BSAs)
    {
        optimizer.process(file);
        ++_numberCompletedFiles;
        printProgress(BSAs.size(), "Extracting BSAs");
    }

    //Listing newly extracted files
    listFiles();

    //Processing animations separately since they cannot be processed in a multithreaded way
    //Using time in order to prevent printing progress too often
    QDateTime time1 = QDateTime::currentDateTime();
    QDateTime time2;
    for (const auto &file : _files)
    {
        optimizer.process(file);
        ++_numberCompletedFiles;
        if (_numberCompletedFiles % 100 == 0)
            printProgress(_numberFiles);
        if (_numberCompletedFiles % 10 == 0)
        {
            time2 = QDateTime::currentDateTime();
            if (time2 > time1.addMSecs(3000))
            {
                printProgress(_numberFiles);
                time1 = time2;
            }
        }
    }

    //Packing BSAs
    if (_profile.getGeneralSettings().bBsaCreate()) {
        _numberCompletedFiles = 0;
        printProgress(_modsToProcess.size(), "Packing BSAs");
        for (const auto &folder : _modsToProcess)
        {
            optimizer.packBsa(folder);
            ++_numberCompletedFiles;
            printProgress(_modsToProcess.size(), "Packing BSAs - Folder:  " + QFileInfo(folder).fileName());
        }
    }

    FilesystemOperations::deleteEmptyDirectories(_profile.getGeneralSettings().sUserPath());
    PLOG_INFO << "Process completed<br><br><br>";
    emit end();
}
} // namespace CAO
