/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <QButtonGroup>

#include "AdvancedBSAModule.hpp"
#include "Settings/GeneralSettings.hpp"
#include "Settings/PatternSettings.hpp"
#include "Utils.hpp"
#include "ui_AdvancedBSAModule.h"

namespace CAO {
AdvancedBSAModule::AdvancedBSAModule(QWidget *parent)
    : IWindowModule(parent)
    , ui_(std::make_unique<Ui::AdvancedBSAModule>())
{
    ui_->setupUi(this);

    connectGroupBox(ui_->createTexturesBSAGroupBox, ui_->maxTexturesSize);
}

AdvancedBSAModule::~AdvancedBSAModule() = default;

void AdvancedBSAModule::setUIData([[maybe_unused]] const PatternSettings &pSets, const GeneralSettings &gSets)
{
    ui_->BSAExtract->setChecked(!gSets.bBSACreate());
    ui_->BSACreate->setChecked(gSets.bBSACreate());
    ui_->BSAProcessContent->setChecked(gSets.bBSAProcessContent());
}

void AdvancedBSAModule::connectAll([[maybe_unused]] PatternSettings &pSets, GeneralSettings &gSets)
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

    connectWrapper(*ui_->dontMergeIncompressible, gSets.bBSADontMergeIncomp);
    connectWrapper(*ui_->dontMergeTextures, gSets.bBSADontMergeTextures);
    connectWrapper(*ui_->dontMakeLoaded, gSets.bBSADontMakeLoaded);
    connectWrapper(*ui_->disallowCompression, gSets.bBSADontCompress);
    connectWrapper(*ui_->maxStandardSize, gSets.iBSAMaxSize);
    connectWrapper(*ui_->maxTexturesSize, gSets.iBSATexturesMaxSize);
}

bool AdvancedBSAModule::isSupportedGame(Games game)
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

QString AdvancedBSAModule::name()
{
    return tr("BSA (General)");
}

} // namespace CAO
