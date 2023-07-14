/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "SelectGPUWindow.hpp"

#include <btu/common/string.hpp>

#include <QRadioButton>

namespace cao {
using btu::tex::CompressionDevice;

SelectGPUWindow::SelectGPUWindow(QWidget *parent)
    : QDialog(parent)
    , ui(std::make_unique<Ui::SelectGPUWindow>())
{
    ui->setupUi(this);

    uint32_t last_gpu = 0;
    auto dev          = CompressionDevice::make(last_gpu);
    while (dev)
    {
        devices_.emplace_back(*std::move(dev));
        last_gpu++;
        dev = CompressionDevice::make(last_gpu);
    }

    auto *layout = new QVBoxLayout;
    for (uint i = 0; i <= last_gpu; i++)
    {
        auto *button = new QRadioButton;
        button->setText(QString::fromStdString(btu::common::as_ascii_string(devices_[i].gpu_name())));
        button->setProperty(property_key, i);
        layout->addWidget(button);
    }
    ui->groupBox->setLayout(layout);
}

std::optional<uint> SelectGPUWindow::getSelectedIndex()
{
    auto buttons  = ui->groupBox->findChildren<QRadioButton *>();
    auto selected = std::find_if(std::cbegin(buttons), std::cend(buttons), [](QRadioButton *button) {
        return button->isChecked();
    });

    if (selected != std::cend(buttons))
        return (*selected)->property(property_key).value<uint>();

    return std::nullopt;
}

void SelectGPUWindow::setSelectedIndex(uint val)
{
    auto buttons  = ui->groupBox->findChildren<QRadioButton *>();
    auto selected = std::find_if(std::cbegin(buttons), std::cend(buttons), [val](QRadioButton *button) {
        return button->property(property_key).value<uint>() == val;
    });

    if (selected != std::cend(buttons))
        (*selected)->setChecked(true);
}
} // namespace cao
