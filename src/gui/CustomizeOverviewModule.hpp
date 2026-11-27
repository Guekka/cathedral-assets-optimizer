/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "IWindowModule.hpp"

class QLabel;

namespace cao {
class CustomizeOverviewModule final : public IWindowModule
{
    Q_OBJECT

public:
    explicit CustomizeOverviewModule(QWidget *parent = nullptr);

    [[nodiscard]] auto name() const noexcept -> QString override;
    void ui_to_settings(Settings &settings) const override;
    [[nodiscard]] auto is_supported_game(btu::Game game) const noexcept -> bool override;

private:
    QLabel *profile_label_ = nullptr;
    QLabel *scope_label_   = nullptr;

    void settings_to_ui(const Settings &settings) override;
};
} // namespace cao
