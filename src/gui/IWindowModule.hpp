/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "settings/base_types.hpp"
#include "settings/settings.hpp"
#include "utils/ConnectionWrapper.hpp"

#include <btu/common/games.hpp>

namespace cao {

class IWindowModule : public QWidget, public ConnectionWrapper
{
    Q_OBJECT

public:
    explicit IWindowModule(QWidget *parent);

    void setup(const Settings &settings);
    [[nodiscard]] virtual auto name() const noexcept -> QString = 0;

    virtual void ui_to_settings(Settings &settings) const = 0;

private:
    virtual void set_ui_data(const Settings &settings) = 0;

    [[nodiscard]] virtual auto is_supported_game(btu::Game game) const noexcept -> bool = 0;
};
} // namespace cao
