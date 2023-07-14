/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https:mozilla.org/MPL/2.0/. */

#include "IntermediateModeModule.hpp"

#include "Utils.hpp"
#include "settings/per_file_settings.hpp"

#include <QButtonGroup>

namespace cao {
IntermediateModeModule::IntermediateModeModule(QWidget *parent)
    : IWindowModule(parent)
    , ui_(std::make_unique<Ui::IntermediateModeModule>())
{
    ui_->setupUi(this);

    setData(*ui_->TexturesResizingMode, tr("Disabled"), TextureResizingMode::None);
    setData(*ui_->TexturesResizingMode, tr("By ratio"), TextureResizingMode::ByRatio);
    setData(*ui_->TexturesResizingMode, tr("By fixed size"), TextureResizingMode::BySize);
}

auto IntermediateModeModule::name() const noexcept -> QString
{
    return tr("Intermediate Mode");
}

void IntermediateModeModule::set_ui_data(const Settings &settings)
{
    /* TODO
    ui_->BSAExtract->setChecked(!gSets.bsa_create());
    ui_->BSACreate->setChecked(gSets.bsa_create());
    ui_->BSAProcessContent->setChecked(gSets.bsa_process_content());
     
     auto *bsaButtonGroup = new QButtonGroup(this);
bsaButtonGroup->addButton(ui_->BSACreate);
bsaButtonGroup->addButton(ui_->BSAExtract);
bsaButtonGroup->addButton(ui_->BSAProcessContent);

connect(bsaButtonGroup,
      QOverload<QAbstractButton *>::of(&QButtonGroup::buttonPressed),
      this,
      [this, &gSets](QAbstractButton *button) {
          gSets.bsa_create         = false;
          gSets.bsa_extract        = false;
          gSets.bsa_process_content = false;

          if (button == ui_->BSACreate)
              gSets.bsa_create = true;
          else if (button == ui_->BSAExtract)
              gSets.bsa_extract = true;
          else if (button == ui_->BSAProcessContent)
              gSets.bsa_process_content = true;
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
    */
}
void IntermediateModeModule::ui_to_settings(Settings &settings) const {}
bool IntermediateModeModule::is_supported_game(btu::Game game) const noexcept
{
    return true;
}

} // namespace cao
