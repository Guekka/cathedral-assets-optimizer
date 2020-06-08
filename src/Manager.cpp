/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Manager.hpp"
#include "Utils/Algorithms.hpp"
#include "Utils/wildcards.hpp"

namespace CAO {
Manager::Manager()
{
    //Preparing logging
    initCustomLogger(currentProfile().logPath(), currentProfile().getGeneralSettings().bDebugLog());

    PLOG_VERBOSE << "Checking settings...";

    auto checkSettings = [](const Settings &sets) {
        const std::optional<QString> error = sets.isValid();
        if (error.has_value())
        {
            PLOG_FATAL << error.value();
            throw std::runtime_error("Options are not valid." + error->toStdString());
        }
    };

    checkSettings(currentProfile().getFileTypes());
    checkSettings(currentProfile().getGeneralSettings());
    currentProfile().getPatterns().get() | rx::transform([](auto &&p) { return p.second; })
        | rx::for_each(checkSettings);

    PLOG_INFO << "Listing files and directories...";
    listDirectories();
    listFiles();
}

void Manager::listDirectories()
{
    _modsToProcess.clear();

    const auto &settings = currentProfile().getGeneralSettings();
    if (settings.eMode() == SingleMod)
        _modsToProcess << settings.sUserPath();

    else
    {
        const QDir dir(settings.sUserPath());
        auto isNotSeparator = [](auto &&str) { return !str.contains("separator"); };

        auto isNotBlacklist = [](const QString &dirName) {
            auto &ft = currentProfile().getFileTypes();
            return !ft.match(ft.slModsBlacklist(), dirName);
        };

        dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)
            | rx::filter(isNotSeparator) //TODO add separators to blacklist
            | rx::filter(isNotBlacklist) | rx::append(_modsToProcess);
    }
}

void Manager::printProgress(int total, const QString &text = "Processing files")
{
    emit progressBarTextChanged(text + "- %v/%m - %p%", total, _numberCompletedFiles);
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

            const auto &settings = currentProfile().getSettings(filename);

            const bool processMeshes = settings.bMeshesResave() || settings.iMeshesOptimizationLevel();
            const bool mesh          = filename.endsWith(".nif", Qt::CaseInsensitive) && processMeshes;

            const bool processTextures = settings.bTexturesMipmaps() || settings.bTexturesCompress()
                                         || settings.bTexturesNecessary()
                                         || settings.eTexturesResizingMode() != None;

            const bool texture = (filename.endsWith(".dds", Qt::CaseInsensitive)
                                  || filename.endsWith(".tga", Qt::CaseInsensitive))
                                 && processTextures;

            const bool animation = settings.bAnimationsOptimization()
                                   && filename.endsWith(".hkx", Qt::CaseInsensitive);

            const bool isBsa = filename.endsWith(currentProfile().getGeneralSettings().sBSAExtension(),
                                                 Qt::CaseInsensitive);

            QStringList &list = isBsa ? BSAs : _files;
            if (!mesh && !texture && !animation && !isBsa)
                continue;

            list << filename;
            ++_numberFiles;
        }
    }
}

void Manager::runOptimization()
{
    PLOG_DEBUG << "Profile directory: " << currentProfile().profileDirectory().path();
    PLOG_INFO << "Processing: " + currentProfile().getGeneralSettings().sUserPath();
    PLOG_INFO << "Beginning...";

    MainOptimizer optimizer;

    //Extracting BSAs
    for (const auto &file : BSAs)
    {
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
        time2 = QDateTime::currentDateTime();
        if (time2 > time1.addMSecs(3000))
        {
            printProgress(_numberFiles);
            time1 = time2;
        }
    }

    //Packing BSAs
    _numberCompletedFiles = 0;
    printProgress(_modsToProcess.size(), "Packing BSAs");
    for (const auto &folder : _modsToProcess)
    {
        optimizer.packBsa(folder);
        ++_numberCompletedFiles;
        printProgress(_modsToProcess.size(), "Packing BSAs - Folder:  " + QFileInfo(folder).fileName());
    }

    Filesystem::deleteEmptyDirectories(currentProfile().getGeneralSettings().sUserPath());
    PLOG_INFO << "Process completed<br><br><br>";
    emit end();
}
} // namespace CAO
