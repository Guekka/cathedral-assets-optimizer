/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "ui_SelectGPUWindow.h"

#include <btu/tex/compression_device.hpp>

#include <QDialog>
#include <optional>

namespace Ui {
class SelectGPUWindow;
} // namespace Ui

namespace cao {
class SelectGPUWindow : public QDialog
{
    Q_OBJECT

public:
    static inline constexpr auto property_key = "Index";
    explicit SelectGPUWindow(QWidget *parent = nullptr);
    std::optional<uint> getSelectedIndex();
    void setSelectedIndex(uint val);

private:
    std::unique_ptr<Ui::SelectGPUWindow> ui;
    std::vector<btu::tex::CompressionDevice> devices_;
};
} // namespace cao
