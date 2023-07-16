/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "IWindowModule.hpp"

namespace cao {

IWindowModule::IWindowModule(QWidget *parent)
    : QWidget(parent)
{
}

void IWindowModule::setup(const Settings &settings)
{
    if (!is_supported_game(settings.current_profile().target_game))
    {
        this->setDisabled(true);
        return;
    }
    this->setEnabled(true);

    set_ui_data(settings);
}

} // namespace cao
