/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "BSA.hpp"
#include "Commands/Plugins/PluginsOperations.hpp"

namespace CAO {
BSA BSA::getBSA(const BSAType &type, const Settings &settings)
{
    BSA bsa;

    switch (type)
    {
        case StandardBsa:
            bsa.type = BSAType::StandardBsa;
            bsa.maxSize = settings.getValue<double>(iBSAMaxSize);
            bsa.format = settings.getValue<bsa_archive_type_t>(eBSAFormat);
            break;
        case TexturesBsa:
            bsa.type = BSAType::TexturesBsa;
            bsa.maxSize = settings.getValue<double>(iBSATexturesMaxSize);
            bsa.format = settings.getValue<bsa_archive_type_t>(eBSATexturesFormat);
            break;
        case UncompressableBsa:
            bsa.type = BSAType::UncompressableBsa;
            bsa.maxSize = settings.getValue<double>(iBSAMaxSize);
            bsa.format = settings.getValue<bsa_archive_type_t>(eBSAFormat);
            break;
    }
    return bsa;
}

void BSA::name(const QString &folder, const Settings &settings)
{
    const auto &bsaSuffix = settings.getValue<QString>(sBSASuffix);
    const auto &bsaTexSuffix = settings.getValue<QString>(sBSATexturesSuffix);
    const QString &suffix = type == TexturesBsa ? bsaTexSuffix : bsaSuffix;
    path = folder + "/" + PluginsOperations::findPlugin(folder, type, settings) + suffix;
    PLOG_VERBOSE << "Named " << type << path;
}

size_t BSA::mergeBSAs(std::vector<BSA> &list)
{
    size_t counter = 0;
    for (auto leftBSA = list.begin(); leftBSA != list.end(); ++leftBSA)
    {
        for (auto rightBSA = list.begin(); rightBSA != list.end(); ++rightBSA)
        {
            if (leftBSA == rightBSA)
                continue;

            if (leftBSA->filesSize + rightBSA->filesSize > leftBSA->maxSize)
                continue; //These BSAs contain too much files to be merged

            if (leftBSA->type != rightBSA->type  //We can only merge BSAs of the same type
                && leftBSA->type != StandardBsa) //But everything can be merged into a standard BSA
                continue;

            *leftBSA += *rightBSA;
            rightBSA->files.clear(); //Marking for deletion
            PLOG_VERBOSE << "Merged " << rightBSA->type << " into " << leftBSA->type;
            ++counter;
        }
    }

    //Removing empty BSAs
    [[maybe_unused]] auto a = std::remove_if(list.begin(), list.end(), [](BSA &bsa) { return bsa.files.isEmpty(); });

    PLOG_VERBOSE << "Merged " << counter << " BSAs";
    return counter;
}

BSA &BSA::operator+(const BSA &other)
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

BSA &BSA::operator+=(const BSA &other)
{
    *this = *this + other;
    return *this;
}
bool BSA::operator==(const BSA &other)
{
    return path == other.path && filesSize == other.filesSize && files == other.files
           && qFuzzyCompare(maxSize, other.maxSize) && type == other.type && format == other.format;
}
} // namespace CAO
