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
            bsa.maxSize = settings.getMandatoryValue<int>(AdvancedSettings::iBSAMaxSize);
            bsa.format = settings.getMandatoryValue<bsa_archive_type_t>(AdvancedSettings::eBSAFormat);
            break;
        case TexturesBsa:
            bsa.type = BSAType::TexturesBsa;
            bsa.maxSize = settings.getMandatoryValue<int>(AdvancedSettings::iBSATexturesMaxSize);
            bsa.format = settings.getMandatoryValue<bsa_archive_type_t>(AdvancedSettings::eBSATexturesFormat);
            break;
        case UncompressableBsa:
            bsa.type = BSAType::UncompressableBsa;
            bsa.maxSize = settings.getMandatoryValue<int>(AdvancedSettings::iBSAMaxSize);
            bsa.format = settings.getMandatoryValue<bsa_archive_type_t>(AdvancedSettings::eBSAFormat);
            break;
    }
    return bsa;
}

void BSA::nameBSA(std::initializer_list<BSA *> bsaList, const QString &folder, const Settings &settings)
{
    const auto &bsaSuffix = settings.getMandatoryValue<QString>(AdvancedSettings::sBSASuffix);
    const auto &bsaTexSuffix = settings.getMandatoryValue<QString>(AdvancedSettings::sBSATexturesSuffix);
    for (auto bsa : bsaList)
    {
        const QString &suffix = bsa->type == TexturesBsa ? bsaTexSuffix : bsaSuffix;
        bsa->path = folder + "/" + PluginsOperations::findPlugin(folder, bsa->type, settings) + suffix;
        PLOG_VERBOSE << "Named " << bsa->type << bsa->path;
    }
}

size_t BSA::mergeBSAs(QVector<BSA> &list)
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
                list[k].files.clear();
                PLOG_VERBOSE << "Merged " << list.at(k).type << " into " << list.at(j).type;
                ++counter;
            }
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
