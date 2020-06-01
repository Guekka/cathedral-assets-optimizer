/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "ui_LevelSelector.h"
#include <QDialog>

namespace Ui {
class LevelSelector;
}

namespace CAO {
class MainWindow;
class LevelSelector : public QDialog
{
    Q_OBJECT

public:
    explicit LevelSelector(MainWindow &mw);

private:
    static constexpr inline int easyLevel = 1;
    static constexpr inline int hardLevel = 2;

    void setupWindow(MainWindow &mw, int level);
    QString getHelpText(int level);

private:
    std::unique_ptr<Ui::LevelSelector> ui_;
};
} // namespace CAO
