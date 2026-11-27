/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "CustomizeOverviewModule.hpp"

#include "settings/settings.hpp"
#include "utils/utils.hpp"

#include <QLabel>
#include <QVBoxLayout>

namespace cao {
CustomizeOverviewModule::CustomizeOverviewModule(QWidget *parent)
    : IWindowModule(parent)
{
    auto *layout = new QVBoxLayout(this);

    profile_label_ = new QLabel(this);
    profile_label_->setWordWrap(true);
    layout->addWidget(profile_label_);

    scope_label_ = new QLabel(this);
    scope_label_->setWordWrap(true);
    layout->addWidget(scope_label_);

    auto *description
        = new QLabel(tr("Choose a file rule on the left to edit its settings in the operation tabs. Rules are "
                        "checked from top to bottom; only the first matching rule applies. Files with no match "
                        "use the default settings. BSA (Profile-wide) applies across the profile."),
                     this);
    description->setWordWrap(true);
    layout->addWidget(description);
    layout->addStretch();
}

auto CustomizeOverviewModule::name() const noexcept -> QString
{
    return tr("Overview");
}

void CustomizeOverviewModule::ui_to_settings(Settings &) const
{
}

auto CustomizeOverviewModule::is_supported_game(btu::Game) const noexcept -> bool
{
    return true;
}

void CustomizeOverviewModule::settings_to_ui(const Settings &settings)
{
    profile_label_->setText(
        tr("Profile %1 targets %2.")
            .arg(to_qstring(settings.current_profile_name()),
                 game_display_name(settings.current_profile().target_game)));

    const auto &selected_settings = current_per_file_settings(settings);
    if (selected_settings.pattern.text() == k_default_pattern.text())
        scope_label_->setText(tr("The operation tabs are editing Default file settings."));
    else
    {
        scope_label_->setText(
            tr("The operation tabs are editing settings for %1.")
                .arg(pattern_display_name(selected_settings.pattern)));
    }
}
} // namespace cao
