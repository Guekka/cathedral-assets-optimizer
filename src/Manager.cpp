/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Manager.hpp"
namespace CAO {
Manager::Manager(const Profile &profile)
    : _profile(profile)

{
    //Preparing logging
    initCustomLogger(_profile.logPath(), _profile.getGeneralSettings().bDebugLog());

    PLOG_VERBOSE << "Checking settings...";

    auto checkSettings = [](const Settings &sets) {
        const std::optional<QString> error = sets.isValid();
        if (!error.has_value())
        {
            PLOG_FATAL << error.value();
            throw std::runtime_error("Options are not valid." + error->toStdString());
        }
    };

    checkSettings(_profile.getFileTypes());
    checkSettings(_profile.getGeneralSettings());
    _profile.getPatterns().get() | rx::transform([](auto &&p) { return p.second; })
        | rx::for_each(checkSettings);

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

    else {
        const QDir dir(settings.sUserPath());
        auto isNotSeparator = [](auto &&str) { return !str.contains("separator"); };
        auto isNotIgnored   = [this](auto &&str) { return !_ignoredMods.contains(str, Qt::CaseInsensitive); };
        dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot) >>= pipes::filter(isNotSeparator)
            >>= pipes::filter(isNotIgnored) >>= pipes::push_back(_modsToProcess);
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

    for (auto &subDir : _modsToProcess) {
        QDirIterator it(subDir, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            const QString &filename = it.next();

            if (it.fileInfo().size() == 0)
                continue;

            const auto &settings = _profile.getSettings(filename);

            const bool processMeshes = settings.bMeshesResave()
                                       || settings.iMeshesOptimizationLevel();
            const bool mesh = filename.endsWith(".nif", Qt::CaseInsensitive) && processMeshes;

            const bool processTextures = settings.bTexturesMipmaps() || settings.bTexturesCompress()
                                         || settings.bTexturesNecessary()
                                         || settings.eTexturesResizingMode() != None;

            const bool texture = (filename.endsWith(".dds", Qt::CaseInsensitive)
                                  || filename.endsWith(".tga", Qt::CaseInsensitive))
                                 && processTextures;

            const bool animation = settings.bAnimationsOptimization()
                                   && filename.endsWith(".hkx", Qt::CaseInsensitive);

            const bool isBsa = filename.endsWith(_profile.getGeneralSettings().sBSAExtension(),
                                                 Qt::CaseInsensitive);

            QStringList &list = isBsa ? BSAs : _files;
            if (!mesh && !texture && !animation && !isBsa)
                continue;

            list << filename;
            ++_numberFiles;
        }
    }
}

void Manager::readIgnoredMods()
{
    QFile &&ignoredModsFile = _profile.getFile("ignoredMods.txt");
    _ignoredMods = Filesystem::readFile(ignoredModsFile);

    if (_ignoredMods.isEmpty()) {
        PLOG_WARNING << "ignoredMods.txt not found. All mods will be processed, including tools "
                        "such as Nemesis or "
                        "Bodyslide studio.";
    }
}

void Manager::runOptimization()
{
    PLOG_DEBUG << "Profile directory: " << currentProfile().profileDirectory().path();
    PLOG_INFO << "Processing: " + _profile.getGeneralSettings().sUserPath();
    PLOG_INFO << "Beginning...";

    MainOptimizer optimizer(_profile);

    //Extracting BSAs
    for (const auto &file : BSAs) {
        optimizer.process(file);
        ++_numberCompletedFiles;
        printProgress(BSAs.size(), "Extracting BSAs");
    }

    //Listing newly extracted files
    listFiles();

    //Using time in order to prevent printing progress too often
    QDateTime time1 = QDateTime::currentDateTime();
    QDateTime time2;
    for (const auto &file : _files) {
        optimizer.process(file);
        ++_numberCompletedFiles;
        time2 = QDateTime::currentDateTime();
        if (time2 > time1.addMSecs(3000)) {
            printProgress(_numberFiles);
            time1 = time2;
        }
    }

    //Packing BSAs
    _numberCompletedFiles = 0;
    printProgress(_modsToProcess.size(), "Packing BSAs");
    for (const auto &folder : _modsToProcess) {
        optimizer.packBsa(folder);
        ++_numberCompletedFiles;
        printProgress(_modsToProcess.size(),
                      "Packing BSAs - Folder:  " + QFileInfo(folder).fileName());
    }

    Filesystem::deleteEmptyDirectories(_profile.getGeneralSettings().sUserPath());
    PLOG_INFO << "Process completed<br><br><br>";
    emit end();
}
} // namespace CAO
