/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "AdvancedTexturesWindow.hpp"
#include "Settings/GeneralSettings.hpp"
#include "Settings/PatternSettings.hpp"
#include "Utils.hpp"
#include "ui_AdvancedTexturesWindow.h"

namespace CAO {
AdvancedTexturesWindow::AdvancedTexturesWindow(QWidget *parent)
    : IWindowModule(parent)
    , ui_(std::make_unique<Ui::AdvancedTexturesWindow>())
{
    ui_->setupUi(this);
}

void AdvancedTexturesWindow::connectAll(PatternSettings &patternSets, GeneralSettings &generalSets)
{
    //Base
    connectWrapper(*ui_->mainNecessary, patternSets.bTexturesNecessary);
    connectWrapper(*ui_->mainCompress, patternSets.bTexturesCompress);
    connectWrapper(*ui_->mainMipMaps, patternSets.bTexturesMipmaps);

    connect(ui_->mainBox, &QGroupBox::toggled, [this](bool state) {
        CAO::setEnabled(state, ui_->mainNecessary, ui_->mainCompress, ui_->mainMipMaps);
    });

    //Resizing
    setData(*ui_->resizingMode, "By ratio", TextureResizingMode::ByRatio);
    setData(*ui_->resizingMode, "By fixed size", TextureResizingMode::BySize);

    connect(ui_->resizingMode,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this, &patternSets](int index) {
                patternSets.eTexturesResizingMode = ui_->resizingMode->itemData(index)
                                                        .value<TextureResizingMode>();
            });

    connectWrapper(*ui_->resizingWidth, patternSets.iTexturesResizingWidth);
    connectWrapper(*ui_->resizingHeight, patternSets.iTexturesResizingHeight);

    connectWrapper(*ui_->resizingMinimumCheckBox, patternSets.bTexturesResizeMinimum);
    connectWrapper(*ui_->resizingMinimumWidth, patternSets.iTexturesMinimumWidth);
    connectWrapper(*ui_->resizingMinimumHeight, patternSets.iTexturesMinimumHeight);

    connectGroupBox(ui_->resizingBox,
                    ui_->resizingMode,
                    ui_->resizingMinimumCheckBox,
                    ui_->resizingMinimumWidth,
                    ui_->resizingMinimumHeight,
                    ui_->resizingWidth,
                    ui_->resizingHeight);
}

QString AdvancedTexturesWindow::name()
{
    return tr("Textures (Patterns)");
}

} // namespace CAO
