/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "pch.hpp"
#include "ui_PatternsManagerWindow.h"

namespace CAO {
class Profile;
class PatternsManagerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PatternsManagerWindow(Profile &profile, QWidget *parent = nullptr);
    void updatePatterns(QComboBox &box);

private:
    std::unique_ptr<Ui::PatternsManagerWindow> ui_;
    Profile &profile;

    void createPattern();
    void setPattern(const QString &name);

    void deleteCurrentPattern();
};
} // namespace CAO
