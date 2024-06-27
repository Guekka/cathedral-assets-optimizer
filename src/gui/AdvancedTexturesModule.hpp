/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "gui/IWindowModule.hpp"

#include <QCoreApplication>

namespace Ui {
class AdvancedTexturesModule;
} // namespace Ui

namespace cao {
class ListDialog;

class AdvancedTexturesModule final : public IWindowModule
{
    Q_OBJECT

public:
    explicit AdvancedTexturesModule(QWidget *parent = nullptr);

    AdvancedTexturesModule(const AdvancedTexturesModule &) = delete;
    AdvancedTexturesModule(AdvancedTexturesModule &&)      = delete;

    auto operator=(const AdvancedTexturesModule &) -> AdvancedTexturesModule & = delete;
    auto operator=(AdvancedTexturesModule &&) -> AdvancedTexturesModule      & = delete;

    ~AdvancedTexturesModule() override;

    [[nodiscard]] auto name() const noexcept -> QString override;

    void ui_to_settings(Settings &settings) const override;

private:
    std::unique_ptr<Ui::AdvancedTexturesModule> ui_;
    std::unique_ptr<ListDialog> texture_format_dialog_;

    void settings_to_ui(const Settings &settings) override;
    [[nodiscard]] auto is_supported_game(btu::Game game) const noexcept -> bool override;
};
} // namespace cao
