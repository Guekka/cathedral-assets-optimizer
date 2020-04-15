/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Profiles.h"
#include "pch.h"

enum BSAType
{
    TexturesBsa = 0,
    StandardBsa,
    UncompressableBsa
};

struct BSA
{
    static BSA getBsa(const BSAType &type);

    QString path;
    qint64 filesSize = 0;
    QStringList files;
    double maxSize = LONG_MAX;
    BSAType type = StandardBsa;
    bsa_archive_type_t format;

    BSA &operator+(const BSA &other);
    BSA &operator+=(const BSA &other);
    bool operator==(const BSA &other);

    /*!
     * \brief Finds a name for a BSA
     * \param bsa The BSA to name
     * \param folder The folder in which the BSA will be
     */
    static void nameBsa(std::initializer_list<BSA *> bsaList, const QString &folder);
    /*!
     * \brief Merges BSAs when possible, according to their max size
     * \param list The list of BSAs to merge
     * \return The number of merges done
     */
    static size_t mergeBsas(QVector<BSA>& list, bool mergeBsa);
};

namespace plog
{
    inline Record &operator<<(Record &record, const BSA &bsa)
    {
        return record << "BSA Structure:\nPath: " + bsa.path + " \nUncompressed files size: "
                      << (bsa.filesSize / GigaByte) << "Gb\nMax size: " << bsa.maxSize / GigaByte
                      << "Gb\nType: " << bsa.type << "\nFormat: " << bsa.format
                      << "\nNumber of files: " << bsa.files.size();
    }

    inline Record &operator<<(Record &record, const BSAType &type)
    {
        switch (type)
        {
            case StandardBsa: return record << "Standard BSA";
            case UncompressableBsa: return record << "Uncompressable BSA";
            case TexturesBsa: return record << "Textures BSA";
        }
        return record << "Unknown BSA type";
    }
} // namespace plog
