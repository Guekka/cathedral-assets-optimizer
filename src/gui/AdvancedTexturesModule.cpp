/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "AdvancedTexturesModule.hpp"

#include "ListDialog.hpp"
#include "settings/per_file_settings.hpp"
#include "ui_AdvancedTexturesModule.h"
#include "utils/utils.hpp"

#include <btu/common/algorithms.hpp>

#include <QDebug>
#include <QListWidgetItem>

namespace cao {
AdvancedTexturesModule::AdvancedTexturesModule(QWidget *parent)
    : IWindowModule(parent)
    , ui_(std::make_unique<Ui::AdvancedTexturesModule>())
    , texture_format_dialog_(std::make_unique<ListDialog>(ListDialog::Sorting::Text))
{
    ui_->setupUi(this);

    connect_group_box(ui_->mainBox, ui_->mainCompress, ui_->mainMipMaps);

    connect_group_box(ui_->resizingBox,
                      ui_->resizingMode,
                      ui_->resizingMinimumCheckBox,
                      ui_->resizingMinimumWidth,
                      ui_->resizingMinimumHeight,
                      ui_->resizingWidth,
                      ui_->resizingHeight);

    set_data(*ui_->resizingMode, "By ratio", TextureResizingMode::ByRatio);
    set_data(*ui_->resizingMode, "By fixed size", TextureResizingMode::BySize);

    // link w and h together if ratio is selected
    connect(ui_->resizingWidth, &QSpinBox::valueChanged, [this](int value) {
        if (ui_->resizingMode->currentData().value<TextureResizingMode>() == TextureResizingMode::ByRatio)
            ui_->resizingHeight->setValue(value);
    });
    connect(ui_->resizingHeight, &QSpinBox::valueChanged, [this](int value) {
        if (ui_->resizingMode->currentData().value<TextureResizingMode>() == TextureResizingMode::ByRatio)
            ui_->resizingWidth->setValue(value);
    });

    const auto force_pow2 = [](QSpinBox *box) {
        connect(box, &QSpinBox::editingFinished, [box] {
            const auto upper = btu::tex::util::upper_pow2(box->value());
            box->setValue(static_cast<int>(upper));
        });
    };

    force_pow2(ui_->resizingWidth);
    force_pow2(ui_->resizingHeight);
    force_pow2(ui_->resizingMinimumWidth);
    force_pow2(ui_->resizingMinimumHeight);

    // disable minimum size if not by ratio
    const auto update_min_size = [this] {
        const auto mode     = ui_->resizingMode->currentData().value<TextureResizingMode>();
        const bool is_ratio = mode == TextureResizingMode::ByRatio;

        ui_->resizingMinimumCheckBox->setEnabled(is_ratio);
        if (!is_ratio)
            ui_->resizingMinimumCheckBox->setChecked(false);

        ui_->resizingMinimumWidth->setEnabled(is_ratio && ui_->resizingMinimumCheckBox->isChecked());
        ui_->resizingMinimumHeight->setEnabled(is_ratio && ui_->resizingMinimumCheckBox->isChecked());
    };

    connect(ui_->resizingMode, &QComboBox::currentIndexChanged, update_min_size);
    connect(ui_->resizingMinimumCheckBox, &QCheckBox::stateChanged, update_min_size);
}

AdvancedTexturesModule::~AdvancedTexturesModule() = default;

auto AdvancedTexturesModule::name() const noexcept -> QString
{
    return tr("Textures (Patterns)");
}

void AdvancedTexturesModule::settings_to_ui(const Settings &settings)
{
    const auto &pfs = current_per_file_settings(settings);

    // Main
    ui_->mainBox->setChecked(pfs.tex_optimize != OptimizeType::None);
    ui_->mainCompress->setChecked(pfs.tex.compress);
    ui_->mainMipMaps->setChecked(pfs.tex.mipmaps);

    // Resizing
    ui_->resizingBox->setChecked(true);
    std::visit(btu::common::Overload{[this](std::monostate) { ui_->resizingBox->setChecked(false); },
                                     [this](btu::tex::util::ResizeRatio ratio) {
                                         const bool success = select_data(*ui_->resizingMode,
                                                                          TextureResizingMode::ByRatio);
                                         assert(success);

                                         ui_->resizingWidth->setValue(ratio.ratio);
                                         ui_->resizingHeight->setValue(ratio.ratio);

                                         ui_->resizingMinimumCheckBox->setChecked(true);

                                         ui_->resizingMinimumWidth->setValue(static_cast<int>(ratio.min.w));
                                         ui_->resizingMinimumHeight->setValue(static_cast<int>(ratio.min.h));
                                     },
                                     [this](btu::tex::Dimension dim) {
                                         const bool success = select_data(*ui_->resizingMode,
                                                                          TextureResizingMode::BySize);
                                         assert(success);

                                         ui_->resizingWidth->setValue(static_cast<int>(dim.w));
                                         ui_->resizingHeight->setValue(static_cast<int>(dim.h));

                                         ui_->resizingMinimumCheckBox->setChecked(false);
                                     }},
               pfs.tex.resize);
}

void AdvancedTexturesModule::ui_to_settings(Settings &settings) const
{
    auto &pfs = current_per_file_settings(settings);

    // Main
    pfs.tex_optimize = ui_->mainBox->isChecked() ? OptimizeType::Normal : OptimizeType::None;
    pfs.tex.compress = ui_->mainCompress->isChecked();
    pfs.tex.mipmaps  = ui_->mainMipMaps->isChecked();

    // Resizing
    pfs.tex.resize = std::monostate{};
    if (ui_->resizingBox->isChecked())
    {
        switch (ui_->resizingMode->currentData().value<TextureResizingMode>())
        {
            case TextureResizingMode::ByRatio:
                pfs.tex.resize = btu::tex::util::ResizeRatio{
                    .ratio = static_cast<uint8_t>(ui_->resizingWidth->value()),
                    .min   = ui_->resizingMinimumCheckBox->isChecked()
                                 ? btu::tex::Dimension{static_cast<size_t>(ui_->resizingMinimumWidth->value()),
                                                     static_cast<size_t>(ui_->resizingMinimumHeight->value())}
                                 : btu::tex::Dimension{0, 0}};
                break;

            case TextureResizingMode::BySize:
                pfs.tex.resize = btu::tex::Dimension{.w = static_cast<size_t>(ui_->resizingWidth->value()),
                                                     .h = static_cast<size_t>(ui_->resizingHeight->value())};
                break;

            case TextureResizingMode::None: break;
        }
    }
}

auto AdvancedTexturesModule::is_supported_game(btu::Game /*game*/) const noexcept -> bool
{
    return true; // even if the game is not supported, the module is probably still useful
}

} // namespace cao
