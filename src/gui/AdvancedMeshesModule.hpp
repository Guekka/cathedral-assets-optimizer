/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "IWindowModule.hpp"

namespace Ui {
class AdvancedMeshesModule;
}

namespace cao {
class AdvancedMeshesModule : public IWindowModule
{
    Q_OBJECT
public:
    explicit AdvancedMeshesModule(QWidget *parent = nullptr);
    ~AdvancedMeshesModule() override;

    [[nodiscard]] auto name() const noexcept -> QString override;

    void ui_to_settings(Settings &settings) const override;

private:
    std::unique_ptr<Ui::AdvancedMeshesModule> ui_;

    void set_ui_data(const Settings &settings) override;
    [[nodiscard]] auto is_supported_game(btu::Game game) const noexcept -> bool override;

    using ConnectionWrapper::connect;
    using ConnectionWrapper::connectWrapper;
};
} // namespace cao
