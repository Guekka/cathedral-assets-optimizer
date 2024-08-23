/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "ModuleDisplay.hpp"

namespace cao {

void ModuleDisplay::set_tab_widget(QTabWidget *tab_widget)
{
    tab_widget_ = tab_widget;
    clear_modules();
}

void ModuleDisplay::set_current_index(int idx)
{
    if (idx > tab_widget_->count())
        idx = 0;

    tab_widget_->setCurrentIndex(idx);
}

void ModuleDisplay::add_module(std::unique_ptr<IWindowModule> mod)
{
    tab_widget_->addTab(mod.release(), mod->name()); // ownership is transferred to tabs
    tab_widget_->setCurrentIndex(0);

    tab_widget_->setHidden(false);
}

void ModuleDisplay::clear_modules()
{
    for (auto widget : get_modules())
    {
        widget->deleteLater();
    }
    tab_widget_->clear();
    tab_widget_->setHidden(true);
}

void ModuleDisplay::hide_unsupported(const btu::Game target_game)
{
    for (auto module : get_modules())
    {
        const auto i = tab_widget_->indexOf(module);
        if (module->is_supported_game(target_game))
            tab_widget_->setTabVisible(i, true);
        else
            tab_widget_->setTabVisible(i, false);
    }
}

int ModuleDisplay::current_index() const noexcept
{
    return tab_widget_->currentIndex();
}

auto ModuleDisplay::get_modules() noexcept -> std::vector<IWindowModule *>
{
    const auto children = tab_widget_->findChildren<IWindowModule *>();
    return {children.begin(), children.end()};
}

auto ModuleDisplay::get_modules() const noexcept -> std::vector<const IWindowModule *>
{
    const auto children = tab_widget_->findChildren<const IWindowModule *>();
    return {children.begin(), children.end()};
}

} // namespace cao
