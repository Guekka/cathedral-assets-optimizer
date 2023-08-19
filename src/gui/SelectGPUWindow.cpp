/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "SelectGPUWindow.hpp"

#include "gui/utils/utils.hpp"

#include <btu/common/string.hpp>

#include <QRadioButton>

namespace cao {
using btu::tex::CompressionDevice;

SelectGPUWindow::SelectGPUWindow(QWidget *parent)
    : QDialog(parent)
    , ui_(std::make_unique<Ui::SelectGPUWindow>())
{
    ui_->setupUi(this);

    auto dev = CompressionDevice::make(0);
    while (dev)
    {
        devices_.emplace_back(*std::move(dev));
        dev = CompressionDevice::make(devices_.size());
    }

    auto *layout = new QVBoxLayout(this); // NOLINT(cppcoreguidelines-owning-memory
    for (size_t i = 0; i <= devices_.size(); i++)
    {
        auto *button = new QRadioButton(this); // NOLINT(cppcoreguidelines-owning-memory)
        button->setText(to_qstring(devices_[i].gpu_name()));
        button->setProperty(property_key, QVariant::fromValue(i));
        layout->addWidget(button);
    }
    ui_->groupBox->setLayout(layout);
}

auto SelectGPUWindow::get_selected_index() -> std::optional<size_t>
{
    const auto buttons  = ui_->groupBox->findChildren<QRadioButton *>();
    const auto selected = std::ranges::find_if(buttons, &QRadioButton::isChecked);

    if (selected != std::cend(buttons))
        return (*selected)->property(property_key).value<size_t>();

    return std::nullopt;
}

void SelectGPUWindow::set_selected_index(size_t val)
{
    auto buttons  = ui_->groupBox->findChildren<QRadioButton *>();
    auto selected = std::find_if(std::cbegin(buttons), std::cend(buttons), [val](QRadioButton *button) {
        return button->property(property_key).value<size_t>() == val;
    });

    if (selected != std::cend(buttons))
        (*selected)->setChecked(true);
}
} // namespace cao
