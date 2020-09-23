/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <iosfwd>
#include <vector>

#include <QString>
#include <QStringList>

#include "Settings/BaseTypes.hpp"
#include "libbsarch/src/libbsarch.h"

namespace CAO {
enum BSAType
{
    TexturesBsa = 0,
    StandardBsa,
    UncompressableBsa
};

class GeneralSettings;
struct BSA
{
    static BSA getBSA(const BSAType &type, const GeneralSettings &settings);

    explicit BSA() = default;
    explicit BSA(double maxSize, qint64 size, BSAType type);

    long long int filesSize = 0;
    double maxSize          = LONG_MAX;
    QString path;
    QStringList files{};
    BSAType type = StandardBsa;
    bsa_archive_type_t format{};

    BSA &operator+=(const BSA &other);
    BSA operator+(const BSA &other) const;
    bool operator==(const BSA &other) const;

    /*!
     * \brief Finds a name for a BSA
     * \param folder The folder in which the BSA will be
     * \param settings CAO Settings
     */
    void name(const QString &folder, const GeneralSettings &settings);

    enum class MergeBSATypes
    {
        None,
        Incompressible,
        Textures,
        Both
    };

    //! \brief Merges BSAs when possible, according to their max size
    static void mergeBSAs(std::vector<BSA> &list, MergeBSATypes mergeTypes);
};

std::ostream &operator<<(std::ostream &os, const BSA &bsa);
std::ostream &operator<<(std::ostream &os, const BSAType &type);
} // namespace CAO
