/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "IWindowModule.hpp"

#include <btu/common/games.hpp>

namespace Ui {
class AdvancedAnimationsModule;
}

namespace cao {
class AdvancedAnimationsModule : public IWindowModule
{
    Q_OBJECT

public:
    explicit AdvancedAnimationsModule(QWidget *parent = nullptr);
    ~AdvancedAnimationsModule() override;

    [[nodiscard]] auto name() const noexcept -> QString override;

private:
    std::unique_ptr<Ui::AdvancedAnimationsModule> ui_;

    void set_ui_data(const Settings &settings) override;
    void ui_to_settings(Settings &settings) const override;
    [[nodiscard]] auto is_supported_game(btu::Game game) const noexcept -> bool override;

    using ConnectionWrapper::connect;
    using ConnectionWrapper::connectWrapper;
};
} // namespace cao