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

    enum OptimizationPhase
    {
        BSAExtraction,
        FileOptimization,
        BSAPacking
    };

public:
    explicit Manager();

    void runOptimization();
    void emitProgress(const QString &modName, OptimizationPhase phase, int currModIndex);

private:
    void listDirectories();
    std::vector<ModFolder> mods_;
    QString phaseToString(OptimizationPhase phase);

signals:
    void progressBarTextChanged(QString text, int maximum, int value);
    void end();
};
} // namespace CAO
