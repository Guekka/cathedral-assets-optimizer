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

void ModuleDisplay::add_module(std::unique_ptr<IWindowModule> mod)
{
    tab_widget_->addTab(mod.release(), mod->name()); // ownership is transferred to tabs
    tab_widget_->setCurrentIndex(0);

    tab_widget_->setHidden(false);
}

void ModuleDisplay::clear_modules()
{
    tab_widget_->clear();
    tab_widget_->setHidden(true);
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
