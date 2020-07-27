/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "MainOptimizer.hpp"
#include "ModFolder.hpp"
#include "Utils/Filesystem.hpp"
#include "pch.hpp"

namespace CAO {
class Manager final : public QObject
{
    Q_OBJECT

public:
    explicit Manager();

    void runOptimization();
    void emitProgress(const QString &modName, CommandType type, int completedFiles, int totalFiles);

private:
    void listDirectories();
    QString getOutputRootDirectory(const QString &inputDirectory);

    QString fileTypeToString(CommandType type);

    std::vector<ModFolder> mods_;
    const QString creationDate_;

signals:
    void progressBarTextChanged(QString text, int maximum, int value);
    void end();
};
} // namespace CAO
