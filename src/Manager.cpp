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
    const auto &settings = currentProfile().getGeneralSettings();
    mods_.clear();
    if (settings.eMode() == SingleMod)
    {
        mods_.emplace_back(ModFolder{settings.sUserPath(), settings.sBSAExtension()});
    }
    else
    {
        const QDir dir(settings.sUserPath());

        auto notBlacklist = [](const QString &dirName) {
            auto &ft = currentProfile().getFileTypes();
            return !ft.match(ft.slModsBlacklist(), dirName);
        };

        mods_ = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot) | rx::filter(notBlacklist)
                | rx::transform([&settings](auto &&path) { return ModFolder(path, settings.sBSAExtension()); })
                | rx::to_vector();
    }
}

QString Manager::phaseToString(Manager::OptimizationPhase phase)
{
    switch (phase)
    {
        case BSAExtraction: return "Extracting BSAs";
        case FileOptimization: return "Processing files";
        case BSAPacking: return "Packing BSAs";
        default: return "Unknown phase";
    }
}

void Manager::emitProgress(const QString &modName, OptimizationPhase phase, int currModIndex)
{
    const auto &text = QString("Processing mod: '%1'. %2 - %v/%m - %p%").arg(modName, phaseToString(phase));
    emit progressBarTextChanged(text, mods_.size(), currModIndex);
}

void Manager::runOptimization()
{
    PLOG_DEBUG << "Profile directory: " << currentProfile().profileDirectory().path();
    PLOG_INFO << "Processing: " + currentProfile().getGeneralSettings().sUserPath();
    PLOG_INFO << "Beginning...";

    getProfiles().beginRun();

    MainOptimizer optimizer;

    for (uint i = 0; i < mods_.size(); i++)
    {
        auto &mod = mods_[i];
        mod.load();
        emitProgress(mod.name(), BSAExtraction, i);
        bool progressEmitted = false;

        while (mod.hasNext())
        {
            auto file = mod.consume();
            if (file->type() == CommandType::BSAFile)
            {
                optimizer.extractBSA(*file);
            }
            else
            {
                if (!progressEmitted)
                    emitProgress(mod.name(), FileOptimization, i);
                optimizer.process(*file);
            }
        }

        //Packing BSAs
        emitProgress(mod.name(), BSAPacking, i);
        optimizer.packBsa(mod.path());
    }

    Filesystem::deleteEmptyDirectories(currentProfile().getGeneralSettings().sUserPath(),
                                       currentProfile().getFileTypes());

    PLOG_INFO << "Process completed\n\n\n";
    emit end();
}
} // namespace CAO
