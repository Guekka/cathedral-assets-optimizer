/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"

#ifdef __GNUC__
#define PACKED(datastructure) datastructure __attribute__((__packed__))
#else
#define PACKED(datastructure) __pragma(pack(push, 1)) datastructure __pragma(pack(pop))
#endif

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

class PluginsOperations final : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(PluginsOperations)

public:
    /*!
   * \brief listHeadparts List all the headparts in a plugin file
   * \param filepath The path of the file to scan
   * \return The list of headparts
   */
    static QStringList listHeadparts(const QString &filepath);

    static QStringList listLandscapeTextures(const QString &filepath);
};
