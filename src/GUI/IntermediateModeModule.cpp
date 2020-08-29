/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "IntermediateModeModule.hpp"
#include "Settings/GeneralSettings.hpp"
#include "Settings/PatternSettings.hpp"
#include "Utils.hpp"

namespace CAO {
IntermediateModeModule::IntermediateModeModule(QWidget *parent)
    : IWindowModule(parent)
    , ui_(std::make_unique<Ui::IntermediateModeModule>())
{
    ui_->setupUi(this);
}

void IntermediateModeModule::init(PatternSettings &pSets, GeneralSettings &gSets)
{
    ui_->BSAExtract->setChecked(!gSets.bBSACreate());
    ui_->BSACreate->setChecked(gSets.bBSACreate());
    ui_->BSAProcessContent->setChecked(gSets.bBSAProcessContent());

    pSets.bTexturesNecessary      = true;
    pSets.bTexturesLandscapeAlpha = true;

    setData(*ui_->TexturesResizingMode, "Disabled", TextureResizingMode::None);
    setData(*ui_->TexturesResizingMode, "By ratio", TextureResizingMode::ByRatio);
    setData(*ui_->TexturesResizingMode, "By fixed size", TextureResizingMode::BySize);
}

void IntermediateModeModule::connectAll(PatternSettings &pSets, GeneralSettings &gSets)
{
    auto *bsaButtonGroup = new QButtonGroup(this);
    bsaButtonGroup->addButton(ui_->BSACreate);
    bsaButtonGroup->addButton(ui_->BSAExtract);
    bsaButtonGroup->addButton(ui_->BSAProcessContent);

    connect(bsaButtonGroup,
            QOverload<QAbstractButton *>::of(&QButtonGroup::buttonPressed),
            this,
            [this, &gSets](QAbstractButton *button) {
                gSets.bBSACreate         = false;
                gSets.bBSAExtract        = false;
                gSets.bBSAProcessContent = false;

                if (button == ui_->BSACreate)
                    gSets.bBSACreate = true;
                else if (button == ui_->BSAExtract)
                    gSets.bBSAExtract = true;
                else if (button == ui_->BSAProcessContent)
                    gSets.bBSAProcessContent = true;
                else
                    throw UiException("Unknown BSA button pressed");
            });

    connectWrapper(*ui_->TexturesCompress, pSets.bTexturesCompress);
    connectWrapper(*ui_->TexturesMipmaps, pSets.bTexturesMipmaps);

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

bool IntermediateModeModule::isSupportedGame(Games game)
{
    switch (game)
    {
        case Games::Morrowind:
        case Games::Oblivion:
        case Games::SkyrimLE:
        case Games::SkyrimSE:
        case Games::Fallout3:
        case Games::FalloutNewVegas:
        case Games::Fallout4: return true;
    }
    return false;
}

QString IntermediateModeModule::name()
{
    return tr("Intermediate mode");
}

} // namespace CAO
