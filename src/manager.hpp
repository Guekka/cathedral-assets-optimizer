/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "settings/base_types.hpp"
#include "settings/settings.hpp"

#include <QObject>
#include <QString>

namespace cao {
/// \brief The Manager class is responsible for the optimization process.
/// It is the main class of the program and the only "backend" class to be used by the GUI.
class Manager final : public QObject
{
    Q_OBJECT

public:
    explicit Manager(Settings settings);

    void run_optimization();

private:
    Settings settings_;

signals:
    void file_counted(size_t count);
    void file_processed(std::filesystem::path relative_path);
    void end();
};
} // namespace cao
