/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings/BaseTypes.hpp"

namespace Ui {
class LevelSelector;
}

namespace CAO {
class MainWindow;
class LevelSelector : private QDialog
{

public:
    explicit LevelSelector();
    ~LevelSelector();
    bool runSelection(MainWindow &window);
    void setHandler(MainWindow &mw);

private:
    //QSlider does not support custom values. Mapping enum to contingent ints
    static constexpr inline std::array guiModes = {std::pair{1, GuiMode::QuickAutoPort},
                                                   std::pair{2, GuiMode::Medium},
                                                   std::pair{3, GuiMode::Advanced}};

    constexpr GuiMode toGuiMode(int mode)
    {
        for (const auto &val : guiModes)
            if (val.first == mode)
                return val.second;
        return GuiMode::Invalid;
    }
    constexpr int toInt(GuiMode mode)
    {
        for (const auto &val : guiModes)
            if (val.second == mode)
                return val.first;
        return 0;
    }

    QString getHelpText(GuiMode level);

    void setupWindow(MainWindow &mw, GuiMode level);

private:
    std::unique_ptr<Ui::LevelSelector> ui_;
};
} // namespace CAO
