/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "BSA.h"
#include "PluginsOperations.h"

Bsa Bsa::getBsa(const BsaType &type)
{
    Bsa bsa;

    switch (type)
    {
        case StandardBsa:
            bsa.type = BsaType::StandardBsa;
            bsa.maxSize = Profiles::maxBsaUncompressedSize();
            bsa.format = Profiles::bsaFormat();
            break;
        case TexturesBsa:
            bsa.type = BsaType::TexturesBsa;
            bsa.maxSize = Profiles::maxBsaTexturesSize();
            bsa.format = Profiles::Profiles::bsaTexturesFormat();
            break;
        case UncompressableBsa:
            bsa.type = BsaType::UncompressableBsa;
            bsa.maxSize = Profiles::maxBsaUncompressedSize();
            bsa.format = Profiles::bsaFormat();
            break;
    }
    return bsa;
}

void Bsa::nameBsa(std::initializer_list<Bsa *> bsaList, const QString &folder)
{
    for (auto bsa : bsaList)
    {
        const QString &suffix = bsa->type == TexturesBsa ? Profiles::bsaTexturesSuffix() : Profiles::bsaSuffix();
        bsa->path = folder + "/" + PluginsOperations::findPlugin(folder, bsa->type) + suffix;
    }
}

size_t Bsa::mergeBsas(QVector<Bsa> &list)
{
    size_t counter = 0;
    for (int j = 0; j < list.size(); ++j)
    {
        for (int k = 0; k < list.size(); ++k)
        {
            if (j == k)
                continue;

            if (list.at(j).filesSize + list.at(k).filesSize < list.at(j).maxSize
                && (list.at(j).type == list.at(k).type || list.at(j).type == StandardBsa))
            {
                list[j] += list[k];
                list.remove(k);
                ++counter;
            }
        }
    }

    //Removing empty BSAs
    for (int j = 0; j < list.size(); ++j)
        if (list.at(j).files.isEmpty())
            list.remove(j);

    return counter;
}

Bsa &Bsa::operator+(const Bsa &other)
{
    filesSize += other.filesSize;
    files += other.files;
    if ((type == TexturesBsa && other.type != TexturesBsa)
        || (type == UncompressableBsa && other.type != UncompressableBsa))
    {
        type = StandardBsa;
    }

    return *this;
}

Bsa &Bsa::operator+=(const Bsa &other)
{
    *this = *this + other;
    return *this;
}
bool Bsa::operator==(const Bsa &other)
{
    return path == other.path && filesSize == other.filesSize && files == other.files
           && qFuzzyCompare(maxSize, other.maxSize) && type == other.type && format == other.format;
}
