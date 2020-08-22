/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MediumModeWindow.hpp"
#include "Settings/GeneralSettings.hpp"
#include "Settings/PatternSettings.hpp"
#include "Utils.hpp"

namespace CAO {
MediumModeWindow::MediumModeWindow(QWidget *parent)
    : IWindowModule(parent)
    , ui_(std::make_unique<Ui::MediumModeWindow>())
{
    ui_->setupUi(this);
}

void MediumModeWindow::connectAll(PatternSettings &pSets, GeneralSettings &gSets)
{
    ui_->BSAExtract->setChecked(!gSets.bBSACreate());
    ui_->BSACreate->setChecked(gSets.bBSACreate());
    ui_->BSAProcessContent->setChecked(gSets.bBSAProcessContent());

    connect(ui_->BSACreate, &QCheckBox::toggled, this, [this, &gSets](bool checked) {
        if (!checked)
            return;

        gSets.bBSACreate         = true;
        gSets.bBSAExtractFromBSA = false;
        gSets.bBSAProcessContent = false;

        ui_->BSACreate->setChecked(true);
        ui_->BSAExtract->setChecked(false);
        ui_->BSAProcessContent->setChecked(false);
    });

    connect(ui_->BSAExtract, &QCheckBox::toggled, this, [this, &gSets](bool checked) {
        if (!checked)
            return;

        gSets.bBSACreate         = false;
        gSets.bBSAExtractFromBSA = true;
        gSets.bBSAProcessContent = false;

        ui_->BSACreate->setChecked(false);
        ui_->BSAExtract->setChecked(true);
        ui_->BSAProcessContent->setChecked(false);
    });

    connect(ui_->BSAProcessContent, &QCheckBox::toggled, this, [this, &gSets](bool checked) {
        if (!checked)
            return;

        gSets.bBSACreate         = false;
        gSets.bBSAExtractFromBSA = false;
        gSets.bBSAProcessContent = true;

        ui_->BSACreate->setChecked(false);
        ui_->BSAExtract->setChecked(false);
        ui_->BSAProcessContent->setChecked(true);
    });

    pSets.bTexturesNecessary      = true;
    pSets.bTexturesLandscapeAlpha = true;

    connectWrapper(*ui_->TexturesCompress, pSets.bTexturesCompress);
    connectWrapper(*ui_->TexturesMipmaps, pSets.bTexturesMipmaps);

    setData(*ui_->TexturesResizingMode, "Disabled", TextureResizingMode::None);
    setData(*ui_->TexturesResizingMode, "By ratio", TextureResizingMode::ByRatio);
    setData(*ui_->TexturesResizingMode, "By fixed size", TextureResizingMode::BySize);

    connect(ui_->TexturesResizingMode,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            &pSets.eTexturesResizingMode,
            [&pSets, this](int idx) {
                const auto data             = ui_->TexturesResizingMode->itemData(idx);
                pSets.eTexturesResizingMode = data.value<TextureResizingMode>();
            });

    connectWrapper(*ui_->TexturesResizingMinimumCheckBox, pSets.bTexturesResizeMinimum);

    connectWrapper(*ui_->TexturesResizingWidth, pSets.iTexturesResizingWidth);
    connectWrapper(*ui_->TexturesResizingHeight, pSets.iTexturesResizingHeight);

    connectWrapper(*ui_->TexturesResizingMinimumWidth, pSets.iTexturesMinimumWidth);
    connectWrapper(*ui_->TexturesResizingMinimumHeight, pSets.iTexturesMinimumHeight);
}

QString MediumModeWindow::name()
{
    return tr("Intermediate mode");
}

} // namespace CAO
