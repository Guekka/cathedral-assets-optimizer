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
class SelectGPUWindow final : public QDialog
{
    Q_OBJECT

public:
    static constexpr auto property_key = "Index";
    explicit SelectGPUWindow(QWidget *parent = nullptr);

    [[nodiscard]] auto get_selected_index() -> std::optional<uint32_t>;
    void set_selected_index(uint32_t val);

private:
    std::unique_ptr<Ui::SelectGPUWindow> ui_;
    std::vector<btu::tex::CompressionDevice> devices_;
};
} // namespace cao
