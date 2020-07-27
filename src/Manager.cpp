/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Manager.hpp"
#include "Utils/Algorithms.hpp"
#include "Utils/wildcards.hpp"

namespace CAO {
Manager::Manager()
    : creationDate_(QDateTime::currentDateTime().toString("yy.MM.dd_hh.mm"))
{
    //Preparing logging
    initCustomLogger(currentProfile().logPath(), getProfiles().commonSettings().bDebugLog());

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
    currentProfile().getPatterns().get() | rx::for_each(checkSettings);

    PLOG_INFO << "Listing directories...";
    listDirectories();
}

void Manager::listDirectories()
{
    mods_.clear();

    const auto &settings = currentProfile().getGeneralSettings();

    if (settings.eMode() == SingleMod)
    {
        const QString &outDir = getOutputRootDirectory(settings.sInputPath());
        mods_.emplace_back(ModFolder{settings.sInputPath(), settings.sBSAExtension(), outDir});
    }
    else
    {
        const QDir dir(settings.sInputPath());

        auto notBlacklist = [](const QString &dirName) {
            auto &ft = currentProfile().getFileTypes();
            return !ft.match(ft.slModsBlacklist(), dirName);
        };

        mods_ = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot) | rx::filter(notBlacklist)
                | rx::transform([this, &settings, &dir](auto &&path) {
                      const QString absoluteInputPath = dir.absoluteFilePath(path);
                      const QString &outDir           = getOutputRootDirectory(absoluteInputPath);
                      return ModFolder(absoluteInputPath, settings.sBSAExtension(), outDir);
                  })
                | rx::to_vector();
    }
}

QString Manager::getOutputRootDirectory(const QString &inputDirectory)
{
    const auto &settings = currentProfile().getGeneralSettings();

    return settings.bEnableOutputPath()
               ? settings.sOutputPath() + '/' + creationDate_ + '/' + QDir(inputDirectory).dirName()
               : inputDirectory;
}

QString Manager::fileTypeToString(CommandType type)
{
    switch (type)
    {
        case CommandType::BSAFile: return "Processing BSAs";
        case CommandType::BSAFolder: return "Packing BSAs";
        default: return "Processing files";
    }
}

void Manager::emitProgress(const QString &modName, CommandType type, int completedFiles, int totalFiles)
{
    const auto &text = QString("Processing mod: '%1'. %2 - %v/%m - %p%").arg(modName, fileTypeToString(type));

    emit progressBarTextChanged(text, totalFiles, completedFiles);
}

void Manager::runOptimization()
{
    PLOG_DEBUG << "Profile directory: " << currentProfile().profileDirectory().path();
    PLOG_INFO << "Processing: " + currentProfile().getGeneralSettings().sInputPath();
    PLOG_INFO << "Beginning...";

    getProfiles().beginRun();

    MainOptimizer optimizer;

    for (auto &mod : mods_)
    {
        mod.load();

        optimizer.disconnect();
        qRegisterMetaType<CommandType>("CommandType"); //Seriously, Qt?
        connect(&optimizer, &MainOptimizer::processingFile, this, [this, &mod](auto type) {
            emitProgress(mod.name(), type, mod.processedFileCount(), mod.totalFileCount());
        });

        while (mod.hasNext())
        {
            auto file = mod.consume();
            optimizer.process(*file, currentProfile().getGeneralSettings().bDryRun());
        }

        //Packing BSAs
        optimizer.packBsa(mod.outPath());
    }

    const auto &sets          = currentProfile().getGeneralSettings();
    const QString &dirToClean = sets.bEnableOutputPath() ? sets.sOutputPath() : sets.sInputPath();

    Filesystem::deleteEmptyDirectories(dirToClean, currentProfile().getFileTypes());

    PLOG_INFO << "Process completed\n\n\n";
    emit end();
}
} // namespace CAO
