/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "IWindowModule.hpp"
#include "btu/common/games.hpp"

#include <QCoreApplication>
#include <QTabWidget>
#include <memory>

namespace cao {
class Manager;
class Settings;

class ModuleDisplay final
{
    Q_DECLARE_TR_FUNCTIONS(ModuleDisplay)

public:
    void set_tab_widget(QTabWidget *tab_widget);
    void set_current_index(int idx);

    void add_module(std::unique_ptr<IWindowModule> mod);
    void clear_modules();

    void hide_unsupported(const btu::Game target_game);

    [[nodiscard]] int current_index() const noexcept;
    [[nodiscard]] auto get_modules() noexcept -> std::vector<IWindowModule *>;
    [[nodiscard]] auto get_modules() const noexcept -> std::vector<const IWindowModule *>;

private:
    QTabWidget *tab_widget_ = nullptr;
};
} // namespace cao
