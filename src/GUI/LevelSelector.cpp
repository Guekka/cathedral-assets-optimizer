/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "LevelSelector.hpp"
#include "GUI/QuickAutoPortWindow.hpp"
#include "MainWindow.hpp"

namespace CAO {
LevelSelector::LevelSelector(MainWindow &mw)
    : QDialog(nullptr, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
    , ui_(new Ui::LevelSelector)
{
    ui_->setupUi(this);

    ui_->label->setText(getHelpText(ui_->levelSlider->value()));

    connect(ui_->levelSlider, &QSlider::valueChanged, this, [this](int val) {
        ui_->label->setText(getHelpText(val));
    });

    connect(ui_->buttonBox, &QDialogButtonBox::accepted, this, [&mw, this] {
        setupWindow(mw, ui_->levelSlider->value());
    });
}

void LevelSelector::setupWindow(MainWindow &mw, int level)
{
    if (level == easyLevel)
    {
        mw.addModule(std::make_unique<QuickAutoPortWindow>(), "Quick Auto Port");
    }
    else if (level == hardLevel)
    {
        throw std::runtime_error("This level has not yet been implemented.");
    }
    else
        throw std::runtime_error("This level does not exist.");
}

QString LevelSelector::getHelpText(int level)
{
    if (level == easyLevel)
    {
        return tr("Quick Auto Port\n"
                  "Quick Auto Port uses default settings for porting a mod from LE to SSE.\n"
                  "It will work for most of the cases and is the recommended way to port a mod.\n"
                  "It is safe to apply it on a mod, and it is recommended to apply it to your whole "
                  "mod list if you experience crashes.");
    }
    else if (level == hardLevel)
    {
        return tr("Advanced mode\n"
                  "The full CAO experience. With profiles and patterns, you can fully customize "
                  "how CAO will optimize your files.");
    }
}
} // namespace CAO
