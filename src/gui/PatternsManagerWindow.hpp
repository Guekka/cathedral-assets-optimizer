/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <QDialog>
class QComboBox;

namespace Ui {
class PatternsManagerWindow;
}

namespace cao {
class Settings;
class PatternsManagerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PatternsManagerWindow(Settings profile, QWidget *parent = nullptr);
    ~PatternsManagerWindow(); // = default

    void updatePatterns(QComboBox &box);

private:
    std::unique_ptr<Ui::PatternsManagerWindow> ui_;
    Settings &profile;

    void createPattern();
    void setPattern(const QString &name);

    void deleteCurrentPattern();
};
} // namespace cao
