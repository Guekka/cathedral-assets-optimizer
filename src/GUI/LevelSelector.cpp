/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "LevelSelector.hpp"
#include "GUI/IntermediateModeModule.hpp"
#include "MainWindow.hpp"
#include "Settings/Profiles.hpp"
#include "ui_LevelSelector.h"

namespace CAO {
LevelSelector::LevelSelector()
    : QDialog(nullptr, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
    , ui_(std::make_unique<Ui::LevelSelector>())
{
    ui_->setupUi(this);

    auto &sets            = getProfiles().commonSettings();
    const GuiMode curMode = sets.bRememberGUIMode() ? sets.eGUIMode() : GuiMode::QuickAutoPort;

    ui_->levelSlider->setValue(toInt(curMode));
    ui_->label->setText(getHelpText(curMode));

    connect(ui_->levelSlider, &QSlider::valueChanged, this, [this](int val) {
        const GuiMode mode = toGuiMode(val);
        ui_->label->setText(getHelpText(mode));
    });
}

LevelSelector::~LevelSelector() = default;

bool LevelSelector::runSelection(MainWindow &window)
{
    if (getProfiles().commonSettings().bRememberGUIMode())
    {
        setupWindow(window, getProfiles().commonSettings().eGUIMode());
        return true;
    }

    if (QDialog::exec() != QDialog::Accepted)
        return false;

    const GuiMode selectedValue = toGuiMode(ui_->levelSlider->value());
    setupWindow(window, selectedValue);

    auto &sets = getProfiles().commonSettings(); //In case they were changed in setupWindow

    if (ui_->rememberChoice->isChecked())
    {
        sets.bRememberGUIMode = true;
        sets.eGUIMode         = selectedValue;
    }
    else
    {
        sets.bRememberGUIMode = false;
    }

    return true;
}

void LevelSelector::setHandler(MainWindow &mw)
{
    mw.setLevelSelectorHandler([this, &mw] {
        mw.hide();
        getProfiles().commonSettings().bRememberGUIMode = false;
        runSelection(mw);
        mw.show();
    });
}

void LevelSelector::setupWindow(MainWindow &mw, GuiMode level)
{
    mw.clearModules();
    switch (level)
    {
        case GuiMode::QuickAutoPort:
        {
            getProfiles() = Profiles(QDir(Profiles::QuickAutoPortProfilesDir));
            mw.initSettings();
            mw.setPatternsEnabled(false);
            break;
        }
        case GuiMode::Medium:
        {
            mw.initSettings();
            mw.addModule(new IntermediateModeModule);
            mw.setPatternsEnabled(false);
            break;
        }
        case GuiMode::Advanced:
        {
            throw std::runtime_error("This level has not yet been implemented.");
        }
        case GuiMode::Invalid: throw std::runtime_error("This level does not exist.");
    }
}

QString LevelSelector::getHelpText(GuiMode level)
{
    switch (level)
    {
        case GuiMode::QuickAutoPort:
        {
            return tr("Quick Auto Port\n"
                      "Quick Auto Port uses default settings for porting a mod between LE and SSE.\n"
                      "It will work for most of the cases and is the recommended way to port a mod.\n"
                      "It is safe to apply it on a mod, and it is recommended to apply it to your whole "
                      "mod list if you experience crashes.");
        }
        case GuiMode::Medium:
        {
            return tr("Intermediate mode\n"
                      "Intermediate mode offers default settings for most use cases, as well as being a bit "
                      "more customizable than Quick Auto Port.");
        }
        case GuiMode::Advanced:
        {
            return tr("Advanced mode\n"
                      "NOT IMPLEMENTED\n"
                      "The full CAO experience. With profiles and patterns, you can fully customize "
                      "how CAO will optimize your files.");
        }
        case GuiMode::Invalid:
        {
            throw std::runtime_error("This level does not exist.");
        }
    }
    return tr("Unknown mode");
}
} // namespace CAO
