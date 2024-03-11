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

class LevelSelector : private QDialog
{
    Q_DECLARE_TR_FUNCTIONS(LevelSelector)
public:
    explicit LevelSelector(GuiSettings settings);

    LevelSelector(const LevelSelector &) = delete;
    LevelSelector(LevelSelector &&)      = delete;

    auto operator=(const LevelSelector &) -> LevelSelector & = delete;
    auto operator=(LevelSelector &&) -> LevelSelector      & = delete;

    ~LevelSelector() override;

    [[nodiscard]] auto run_selection() noexcept -> GuiSettings;

    [[maybe_unused]] auto eventFilter(QObject *obj, QEvent *event) noexcept -> bool override;

private:
    constexpr static inline auto k_button_property_name = "name";

    std::unique_ptr<Ui::LevelSelector> ui_;
    GuiSettings settings_;
};
} // namespace cao
