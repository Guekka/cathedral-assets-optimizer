/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Profiles.hpp"
#include "pch.hpp"

enum BsaType
{
    TexturesBsa = 0,
    StandardBsa,
    UncompressableBsa
};

struct Bsa
{
    static Bsa getBsa(const BsaType &type);

    QString path;
    qint64 filesSize = 0;
    QStringList files;
    double maxSize = LONG_MAX;
    BsaType type = StandardBsa;
    bsa_archive_type_t format;

    Bsa &operator+(const Bsa &other);
    Bsa &operator+=(const Bsa &other);
    bool operator==(const Bsa &other);

    /*!
     * \brief Finds a name for a BSA
     * \param bsa The BSA to name
     * \param folder The folder in which the BSA will be
     */
    static void nameBsa(std::initializer_list<Bsa *> bsaList, const QString &folder);
    /*!
     * \brief Merges BSAs when possible, according to their max size
     * \param list The list of BSAs to merge
     * \return The number of merges done
     */
    static size_t mergeBsas(QVector<Bsa> &list);
};

namespace plog
{
    inline Record &operator<<(Record &record, const Bsa &bsa)
    {
        return record << "BSA Structure:\nPath: " + bsa.path + " \nUncompressed files size: "
                      << (bsa.filesSize / GigaByte) << "Gb\nMax size: " << bsa.maxSize / GigaByte
                      << "Gb\nType: " << bsa.type << "\nFormat: " << bsa.format
                      << "\nNumber of files: " << bsa.files.size();
    }
} // namespace plog
