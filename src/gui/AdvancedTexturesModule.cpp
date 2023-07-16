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
    , textureFormatDialog_(std::make_unique<ListDialog>(ListDialog::doSortByText))
{
    ui_->setupUi(this);

    connectGroupBox(ui_->mainBox,
                    ui_->mainNecessary,
                    ui_->mainCompress,
                    ui_->mainMipMaps,
                    ui_->mainAlphaLandscape);

    connectGroupBox(ui_->resizingBox,
                    ui_->resizingMode,
                    ui_->resizingMinimumCheckBox,
                    ui_->resizingMinimumWidth,
                    ui_->resizingMinimumHeight,
                    ui_->resizingWidth,
                    ui_->resizingHeight);

    setData(*ui_->resizingMode, tr("By ratio"), TextureResizingMode::ByRatio);
    setData(*ui_->resizingMode, tr("By fixed size"), TextureResizingMode::BySize);

    setData(*ui_->outputFormat, tr("BC7 (BC7_UNORM)"), DXGI_FORMAT_BC7_UNORM);
    setData(*ui_->outputFormat, tr("BC5 (BC5_UNORM)"), DXGI_FORMAT_BC5_UNORM);
    setData(*ui_->outputFormat, tr("BC3 (BC3_UNORM)"), DXGI_FORMAT_BC3_UNORM);
    setData(*ui_->outputFormat, tr("BC1 (BC1_UNORM)"), DXGI_FORMAT_BC1_UNORM);
    setData(*ui_->outputFormat, tr("Uncompressed (R8G8B8A8_UNORM)"), DXGI_FORMAT_R8G8B8A8_UNORM);

    //Init unwanted formats
    textureFormatDialog_->setUserAddItemVisible(false);

    /*
    for (const auto &value : Detail::DxgiFormats)
    {
        auto item = new QListWidgetItem(value.name);
        item->setData(Qt::UserRole, value.format);

        textureFormatDialog_->addItem(item);
    }
     FIXME
     */
}

AdvancedTexturesModule::~AdvancedTexturesModule() = default;

QString AdvancedTexturesModule::name() const noexcept
{
    return tr("Textures (Patterns)");
}

void AdvancedTexturesModule::set_ui_data(const Settings &settings)
{
    /* FIXME
    ui_->mainBox->setChecked(pSets.bTexturesNecessary() || pSets.bTexturesCompress()
                             || pSets.bTexturesMipmaps() || pSets.bTexturesLandscapeAlpha());

    //Resizing
    selectData(*ui_->resizingMode, pSets.eTexturesResizingMode());
    ui_->resizingBox->setChecked(pSets.eTexturesResizingMode() != None);

    // selectData(*ui_->outputFormat, pSets.eTexturesFormat());

    //Unwanted formats (note: very inefficient algorithm but dataset is small)
    const auto unwantedFormats = pSets.slTextureUnwantedFormats();
    for (auto *item : textureFormatDialog_->items())
    {

        const auto checkState = btu::common::contains(unwantedFormats, item->data(Qt::UserRole).value<DXGI_FORMAT>())
                                    ? Qt::Checked
                                    : Qt::Unchecked;



        //         item->setCheckState(checkState);
    }

     //Base
// connectWrapper(*ui_->mainNecessary, pSets.bTexturesNecessary);
// connectWrapper(*ui_->mainCompress, pSets.bTexturesCompress);
// connectWrapper(*ui_->mainMipMaps, pSets.bTexturesMipmaps);
// connectWrapper(*ui_->mainAlphaLandscape, pSets.bTexturesLandscapeAlpha);

//Resizing
const auto update_resize_mode = [&pSets, this](int index) {
const auto data = ui_->resizingBox->isChecked()
                     ? ui_->resizingMode->itemData(index).value<TextureResizingMode>()
                     : None;
//pSets. = data; FIXME
};

connect(ui_->resizingMode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, update_resize_mode);

connect(ui_->resizingBox, &QGroupBox::toggled, this, [this, update_resize_mode]() {
update_resize_mode(ui_->resizingMode->currentIndex());
});

// connectWrapper(*ui_->resizingWidth, pSets.iTexturesResizingWidth);
// connectWrapper(*ui_->resizingHeight, pSets.iTexturesResizingHeight);

// connectWrapper(*ui_->resizingMinimumCheckBox, pSets.bTexturesResizeMinimum);
// connectWrapper(*ui_->resizingMinimumWidth, pSets.iTexturesMinimumWidth);
// connectWrapper(*ui_->resizingMinimumHeight, pSets.iTexturesMinimumHeight);

// connectWrapper(*ui_->forceConversion, pSets.bTexturesForceConvert);


connect(ui_->outputFormat,
   QOverload<int>::of(&QComboBox::currentIndexChanged),
   &pSets.eTexturesFormat,
   [&pSets, this](int idx) {
       const auto data       = ui_->outputFormat->itemData(idx);
       pSets.eTexturesFormat = data.value<DXGI_FORMAT>();
   });
FIXME


    connect(ui_->texturesUnwantedFormatsEdit,
            &QPushButton::pressed,
            textureFormatDialog_.get(),
            &ListDialog::open);

connect(textureFormatDialog_.get(), &QDialog::finished, this, [&pSets, this] {
pSets.slTextureUnwantedFormats = textureFormatDialog_->getChoices() | rx::transform([](auto *item) {
                                    return item->data(Qt::UserRole).template value<DXGI_FORMAT>();
                                })
                                | rx::to_vector();
});
FIXME

    */
}

void AdvancedTexturesModule::ui_to_settings(Settings &settings) const {}

auto AdvancedTexturesModule::is_supported_game(btu::Game game) const noexcept -> bool
{
    return true; // even if the game is not supported, the module is probably still useful
}

} // namespace cao
