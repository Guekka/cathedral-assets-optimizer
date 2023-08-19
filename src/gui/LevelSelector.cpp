/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "LevelSelector.hpp"

#include "ui_LevelSelector.h"

#include <utility>

namespace cao {

[[nodiscard]] auto get_help_text(GuiMode level) noexcept -> QString
{
    switch (level)
    {
        case GuiMode::QuickOptimize:
        {
            return QObject::tr(
                R"(QuickOptimize

                Uses default settings for porting a mod between LE and SSE.
                It will work for most of the cases and is the recommended way to port a mod.
                It is safe to apply it on a mod, and it is recommended to apply it to your whole mod list if you experience crashes.)");
        }
        case GuiMode::Medium:
        {
            return QObject::tr(
                R"(Intermediate mode

                Offers default settings for most use cases, as well as being a bit more customizable than QuickOptimize.)");
        }
        case GuiMode::Advanced:
        {
            return QObject::tr(
                R"(Advanced mode

                The full CAO experience. With profiles and patterns, you can fully customize how CAO will optimize your files.)");
        }
    }
    return QObject::tr("Unknown mode");
}

LevelSelector::LevelSelector(GuiSettings settings)
    : QDialog(nullptr, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
    , ui_(std::make_unique<Ui::LevelSelector>())
    , settings_(std::move(settings))
{
    ui_->setupUi(this);

    const GuiMode cur_mode = settings_.remember_gui_mode ? settings_.gui_mode : GuiMode::QuickOptimize;

    ui_->label->setText(get_help_text(cur_mode));

    auto setup_button = [this](QPushButton *button, GuiMode mode) {
        button->installEventFilter(this);
        button->setProperty(k_button_property_name, QVariant::fromValue(mode));
        connect(button, &QPushButton::clicked, this, [this, mode] {
            settings_.gui_mode = mode;
            accept();
        });
    };

    setup_button(ui_->quick_optimize, GuiMode::QuickOptimize);
    setup_button(ui_->medium, GuiMode::Medium);
    setup_button(ui_->advanced, GuiMode::Advanced);
}

LevelSelector::~LevelSelector() = default;

auto LevelSelector::run_selection() noexcept -> GuiSettings
{
    if (settings_.remember_gui_mode)
        return settings_;

    if (QDialog::exec() != QDialog::Accepted)
        return settings_;

    settings_.remember_gui_mode = ui_->rememberChoice->isChecked();

    return settings_;
}

[[maybe_unused]] auto LevelSelector::eventFilter(QObject *obj, QEvent *event) noexcept -> bool
{
    if (event->type() == QEvent::HoverEnter)
    {
        auto *button = qobject_cast<QPushButton *>(obj);
        if (button != nullptr)
        {
            auto level = button->property(k_button_property_name).value<GuiMode>();
            ui_->label->setText(get_help_text(level));
        }
        return true;
    }
    return QDialog::eventFilter(obj, event);
}

} // namespace cao
