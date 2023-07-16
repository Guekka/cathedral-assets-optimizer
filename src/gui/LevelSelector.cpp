/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "LevelSelector.hpp"

#include "MainWindow.hpp"
#include "gui/AdvancedAnimationsModule.hpp"
#include "gui/AdvancedBSAModule.hpp"
#include "gui/AdvancedMeshesModule.hpp"
#include "gui/AdvancedTexturesModule.hpp"
#include "gui/IntermediateModeModule.hpp"
#include "settings/settings.hpp"
#include "ui_LevelSelector.h"

namespace cao {
LevelSelector::LevelSelector(Settings settings)
    : QDialog(nullptr, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
    , ui_(std::make_unique<Ui::LevelSelector>())
    , settings_(std::move(settings))
{
    ui_->setupUi(this);

    const GuiMode cur_mode = settings_.gui.remember_gui_mode ? settings_.gui.gui_mode
                                                             : GuiMode::QuickOptimize;

    ui_->levelSlider->setValue(to_int(cur_mode));
    ui_->label->setText(get_help_text(cur_mode));

    connect(ui_->levelSlider, &QSlider::valueChanged, this, [this](int val) {
        const GuiMode mode = to_gui_mode(val);
        ui_->label->setText(get_help_text(mode));
    });
}

LevelSelector::~LevelSelector() = default;

auto LevelSelector::run_selection(cao::MainWindow &window) -> bool
{
    if (settings_.gui.remember_gui_mode)
    {
        setup_window(window, settings_.gui.gui_mode);
        return true;
    }

    if (QDialog::exec() != QDialog::Accepted)
        return false;

    const GuiMode selected_value = to_gui_mode(ui_->levelSlider->value());
    setup_window(window, selected_value);

    settings_.gui.gui_mode          = selected_value;
    settings_.gui.remember_gui_mode = ui_->rememberChoice->isChecked();

    return true;
}

void LevelSelector::set_handler(MainWindow &mw)
{
    mw.setLevelSelectorHandler([this, &mw] {
        mw.hide();
        settings_.gui.remember_gui_mode = false;
        if (run_selection(mw))
            mw.show();
        else
            QCoreApplication::quit(); // TODO: Do not quit the app
    });
}

void LevelSelector::setup_window(MainWindow &mw, GuiMode level)
{
    mw.clearModules();
    /* FIXME
    switch (level)
    {
        case GuiMode::QuickOptimize:
        {
            getProfiles() = Profiles(QDir(Settings::QuickOptimizeProfilesDir));
            mw.setPatternsEnabled(false);
            mw.initSettings();
            break;
        }
        case GuiMode::Medium:
        {
            mw.addModule(new IntermediateModeModule);
            mw.setPatternsEnabled(false);
            mw.initSettings();
            break;
        }
        case GuiMode::Advanced:
        {
            mw.addModule(new AdvancedBSAModule);
            mw.addModule(new AdvancedMeshesModule);
            mw.addModule(new AdvancedTexturesModule);
            mw.addModule(new AdvancedAnimationsModule);
            mw.setPatternsEnabled(true);
            mw.initSettings();
            break;
        }
        case GuiMode::Invalid: throw std::runtime_error("This level does not exist.");
    }
    */
}

auto LevelSelector::get_help_text(cao::GuiMode level) noexcept -> QString
{
    switch (level)
    {
        case GuiMode::QuickOptimize:
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
                      "The full CAO experience. With profiles and patterns, you can fully customize "
                      "how CAO will optimize your files.");
        }
    }
    return tr("Unknown mode");
}
} // namespace cao
