/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "IWindowModule.hpp"
#include "ui_MeshesAdvancedWindow.h"

namespace Ui {
class MeshesAdvancedWindow;
}

namespace CAO {
class MeshesAdvancedWindow : public IWindowModule
{
public:
    explicit MeshesAdvancedWindow(QWidget *parent = nullptr);

    void connectAll(PatternSettings &pSets, GeneralSettings &gSets) override;

    QString name() override;

private:
    Ui::MeshesAdvancedWindow *ui_;
};
} // namespace CAO
