/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings/Settings.hpp"
#include "pch.hpp"

namespace CAO {
enum BSAType
{
    TexturesBsa = 0,
    StandardBsa,
    UncompressableBsa
};

struct BSA
{
    static BSA getBSA(const BSAType &type, const Settings &settings);

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
     * \param folder The folder in which the BSA will be
     * \param settings CAO Settings
     */
    void name(const QString &folder, const Settings &settings);
    /*!
     * \brief Merges BSAs when possible, according to their max size
     * \param list The list of BSAs to merge
     * \return The number of merges done
     */
    static size_t mergeBSAs(std::vector<BSA> &list);
};
} // namespace CAO

namespace plog {
inline Record &operator<<(Record &record, const CAO::BSA &bsa)
{
    return record << "BSA Structure:\nPath: " + bsa.path + " \nUncompressed files size: "
                  << (bsa.filesSize / CAO::GigaByte) << "Gb\nMax size: " << bsa.maxSize / CAO::GigaByte
                  << "Gb\nType: " << bsa.type << "\nFormat: " << bsa.format
                  << "\nNumber of files: " << bsa.files.size();
}

inline Record &operator<<(Record &record, const CAO::BSAType &type)
{
    switch (type)
    {
        case CAO::StandardBsa: return record << "Standard BSA";
        case CAO::UncompressableBsa: return record << "Uncompressable BSA";
        case CAO::TexturesBsa: return record << "Textures BSA";
    }
    return record << "Unknown BSA type";
}
} // namespace plog
