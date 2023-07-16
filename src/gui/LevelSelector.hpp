/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "settings/base_types.hpp"
#include "settings/settings.hpp"

#include <QCoreApplication>
#include <QDialog>
#include <array>

namespace Ui {
class LevelSelector;
}

namespace cao {
class MainWindow;
class LevelSelector : private QDialog
{
    Q_DECLARE_TR_FUNCTIONS(LevelSelector)
public:
    explicit LevelSelector(Settings settings);
    ~LevelSelector();

    auto run_selection(MainWindow &window) -> bool;
    void set_handler(MainWindow &mw);

private:
    std::unique_ptr<Ui::LevelSelector> ui_;
    Settings settings_;

    // QSlider does not support custom values. Mapping enum to contingent ints
    static constexpr inline std::array gui_modes = {std::pair{1, GuiMode::QuickOptimize},
                                                    std::pair{2, GuiMode::Medium},
                                                    std::pair{3, GuiMode::Advanced}};

    [[nodiscard]] static auto to_gui_mode(int mode)
    {
        for (const auto &val : gui_modes)
            if (val.first == mode)
                return val.second;
        throw std::runtime_error("Invalid mode");
    }
    [[nodiscard]] static auto to_int(GuiMode mode)
    {
        for (const auto &val : gui_modes)
            if (val.second == mode)
                return val.first;
        return 0;
    }

    [[nodiscard]] static auto get_help_text(GuiMode level) noexcept -> QString;

    void setup_window(MainWindow &mw, GuiMode level);
};
} // namespace cao
