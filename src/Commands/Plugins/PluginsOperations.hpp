/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/BSA/Utils/BSA.hpp"
#include "pch.hpp"

namespace CAO {
PACKED(struct PluginHeader {
    char type[4];
    uint32_t groupSize;
    char label[4];
    int32_t groupType;
    unsigned short stamp;
    char vc1;
    char vc2;
    uint32_t unknown;
};)

PACKED(struct RecordHeader {
    char type[4];
    uint32_t dataSize;
    uint32_t flags;
    uint32_t id;
    unsigned short stamp;
    char vc1;
    char vc2;
    uint16_t version;
    uint16_t unknown;
};)

PACKED(union PluginRecordHeader {
    RecordHeader record;
    PluginHeader plugin;
};)

PACKED(struct PluginFieldHeader {
    char type[4];
    uint16_t dataSize;
};)

constexpr char GROUP_TES4[4] = {'T', 'E', 'S', '4'};
constexpr char GROUP_GRUP[4] = {'G', 'R', 'U', 'P'};
constexpr char GROUP_HDPT[4] = {'H', 'D', 'P', 'T'};
constexpr char GROUP_MODL[4] = {'M', 'O', 'D', 'L'};
constexpr char GROUP_LTEX[4] = {'L', 'T', 'E', 'X'};
constexpr char GROUP_TNAM[4] = {'T', 'N', 'A', 'M'};
constexpr char GROUP_TXST[4] = {'T', 'X', 'S', 'T'};
constexpr char GROUP_TX00[4] = {'T', 'X', '0', '0'};

namespace PluginsOperations {

/*!
* \brief Try to find a plugin (.esp ; .esl ; .esm) in the given folder. It will always return an esp that isn't associated to a BSA.
* \param folderPath The folder to check
* \param bsaType The type of BSA to load.
* \return a QString containing the name of the plugin without the extension. If no plugin is found, it will return the name of the directory.
*/
QString findPlugin(const QDir &folderPath, const GeneralSettings &settings);
/*!
* \brief Create enough plugins to load all BSAs
* \param folderPath The folder to create plugins into
*/
void makeDummyPlugins(const QString &folderPath, const GeneralSettings &settings);
/*!
* \brief Check if a bsa already has a plugin to load it
* \param bsaPath The path of the bsa to check
*/
bool checkIfBsaHasPlugin(const QString &bsaPath, const GeneralSettings &settings);

/*!
 * \brief Find all the BSAs names in a directory
 * \param it An iterator to the dir to scan
 * \return A list containing the names of the BSAs found
 */
QStringList listBSAsNames(QDirIterator it, const GeneralSettings &settings);
/*!
* \brief List all the headparts in a plugin file
* \param filepath The path of the plugin to scan
* \return The list of headparts
*/
std::vector<std::string> listHeadparts(const QString &filepath);
/*!
* \brief List all the landscape textures in a plugin file
* \param filepath The path of the plugin to scan
* \return The list of headparts
*/
QStringList listLandscapeTextures(const QString &filepath);
} // namespace PluginsOperations
} // namespace CAO
