/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "ListDialog.hpp"
#include "pch.hpp"

namespace Ui {
class BSAFilesToPack;
}

namespace CAO {
class BSAFilesToPackWidget : public QWidget
{
public:
    explicit BSAFilesToPackWidget(QWidget *parent = nullptr);
    ~BSAFilesToPackWidget();
    Ui::BSAFilesToPack &getUi() { return *ui; }

private:
    Ui::BSAFilesToPack *ui;
    ListDialog *standardList;
    ListDialog *uncompressableList;
    ListDialog *textureList;

    /*!
    \brief List of default textures assets (dds)
    */
    const QStringList texturesAssets{"dds"};
    /*!
    \brief List of default standard assets. Includes uncompressable assets but does not include textures assets 
    */
    const QStringList standardAssets{"png",  "nif", "seq", "pex",       "psc",       "lod",     "fuz", "swf", "hkx",
                                     "tri",  "btr", "bto", "btt",       "lip",       "txt",     "lst", "gid", "bgsm",
                                     "bgem", "xml", "gfx", "dlstrings", "ilstrings", "strings", "tga"};
    /*!
    \brief List of default uncompressable assets (sounds)
    */
    const QStringList uncompressableAssets{"wav", "xwm"};

    const QStringList allAssets = texturesAssets + standardAssets + uncompressableAssets;
};
} // namespace CAO
