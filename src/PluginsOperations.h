/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#include "QLogger/QLogger.h"

enum bsaRequired
{
    texturesBsa = 0,
    standardBsa,
    texturesAndStandardBsa
};

class PluginsOperations : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(PluginsOperations)

public:
    /*!
     * \brief Will try to find a plugin (.esp ; .esl ; .esm) in the given folder. It will always return an esp that isn't associated to a BSA OR a textures BSA.
     * \param folderPath The folder to check
     * \param bsaType The type of BSA to load.
     * \return a QString containing the name of the plugin. If no plugin is found, it will return the name of the directory.
     */
    static QString findPlugin(const QString& folderPath, bsaRequired bsaType);
    /*!
     * \brief Will create enough plugins to load all BSAs
     * \param folderPath The folder to create plugins into
     */
    static void makeDummyPlugins(const QString& folderPath);
    /*!
     * \brief Checks if a bsa already has a plugin to load it
     * \param bsaPath The path of the bsa to check
     */
    static bool checkIfBsaHasPlugin(const QString& bsaPath);
};
