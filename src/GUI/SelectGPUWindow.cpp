/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "SelectGPUWindow.hpp"
#include "Commands/Textures/TextureCompressionDevice.hpp"

namespace CAO {
SelectGPUWindow::SelectGPUWindow(QWidget *parent)
    : QDialog(parent)
    , ui(std::make_unique<Ui::SelectGPUWindow>())
{
    ui->setupUi(this);

    uint lastGPU = 0;
    TextureCompressionDevice device(lastGPU);
    while (device.isValid())
    {
        lastGPU++;
        device = TextureCompressionDevice(lastGPU);
    }
    lastGPU--; //Last one is invalid

    auto layout = new QVBoxLayout;
    for (uint i = 0; i <= lastGPU; i++)
    {
        auto button = new QRadioButton;
        button->setText(TextureCompressionDevice(i).gpuName());
        button->setProperty(propertyKey, i);
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
        return (*selected)->property(propertyKey).value<uint>();

    return std::nullopt;
}

void SelectGPUWindow::setSelectedIndex(uint val)
{
    auto buttons  = ui->groupBox->findChildren<QRadioButton *>();
    auto selected = std::find_if(std::cbegin(buttons), std::cend(buttons), [val](QRadioButton *button) {
        return button->property(propertyKey).value<uint>() == val;
    });

    if (selected != std::cend(buttons))
        (*selected)->setChecked(true);
}
} // namespace CAO
