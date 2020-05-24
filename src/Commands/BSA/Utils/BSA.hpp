/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings/BaseTypes.hpp"
#include "pch.hpp"

namespace CAO {
enum BSAType
{
    TexturesBsa = 0,
    StandardBsa,
    UncompressableBsa
};

class GeneralSettings;
struct BSA
{ //TODO: V802 http://www.viva64.com/en/V802 On 32-bit platform, structure size can be reduced from 40 to 32 bytes by rearranging the fields according to their sizes in decreasing order.
    static BSA getBSA(const BSAType &type, const GeneralSettings &settings);

    explicit BSA() = default;
    explicit BSA(double maxSize, qint64 size, BSAType type);

    QString path;
    qint64 filesSize = 0;
    QStringList files{};
    double maxSize = LONG_MAX;
    BSAType type = StandardBsa;
    bsa_archive_type_t format{};

    BSA &operator+=(const BSA &other);
    BSA operator+(const BSA &other) const;
    bool operator==(const BSA &other) const;

    /*!
     * \brief Finds a name for a BSA
     * \param folder The folder in which the BSA will be
     * \param file.settings() CAO Settings
     */
    void name(const QString &folder, const GeneralSettings &settings);

    static constexpr bool mergeDifferentTypes    = true;
    static constexpr bool separateDifferentTypes = false;

    //! \brief Merges BSAs when possible, according to their max size
    static void mergeBSAs(std::vector<BSA> &list, bool merge);
};

inline std::ostream &operator<<(std::ostream &os, const BSA &bsa)
{
    auto str = QString("Path:'%1'\n"
                       "Size:%2Gb\n"
                       "Type:%3\n"
                       "Format:%4\n")
                   .arg(bsa.path)
                   .arg(bsa.filesSize / GigaByte)
                   .arg(bsa.type)
                   .arg(bsa.format);

    return os << str.toStdString();
}

inline std::ostream &operator<<(std::ostream &os, const BSAType &type)
{
    switch (type)
    {
        case CAO::StandardBsa: return os << "Standard BSA";
        case CAO::UncompressableBsa: return os << "Uncompressible BSA";
        case CAO::TexturesBsa: return os << "Textures BSA";
    }
    return os << "Unknown BSA type";
}
} // namespace CAO
