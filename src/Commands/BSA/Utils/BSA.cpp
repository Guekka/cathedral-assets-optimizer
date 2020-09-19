/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "BSA.hpp"
#include "Commands/Plugins/PluginsOperations.hpp"
#include "Settings/Games.hpp"
#include "Settings/GeneralSettings.hpp"
#include "Utils/Algorithms.hpp"

namespace CAO {
BSA BSA::getBSA(const BSAType &type, const GeneralSettings &settings)
{
    BSA bsa;

    const auto &games = GameSettings::get(settings.eGame());

    switch (type)
    {
        case StandardBsa: {
            bsa.type    = BSAType::StandardBsa;
            bsa.maxSize = settings.iBSAMaxSize() * GigaByte;
            bsa.format  = games.eBSAFormat();
            break;
        }
        case TexturesBsa: {
            const auto &textFormat = games.eBSATexturesFormat();
            if (!textFormat.has_value())
                throw std::runtime_error("This game does not support texture bsa: "
                                         + std::to_string(static_cast<int>(settings.eGame())));

            bsa.type    = BSAType::TexturesBsa;
            bsa.maxSize = settings.iBSATexturesMaxSize() * GigaByte;
            bsa.format  = textFormat.value();
            break;
        }
        case UncompressableBsa: {
            bsa.type    = BSAType::UncompressableBsa;
            bsa.maxSize = settings.iBSAMaxSize() * GigaByte;
            bsa.format  = games.eBSAFormat();
            break;
        }
    }
    return bsa;
}

BSA::BSA(double maxSize, qint64 size, BSAType type)
    : filesSize(size)
    , maxSize(maxSize)
    , type(type)
{
}

void BSA::name(const QString &folder, const GeneralSettings &settings)
{
    const auto &games        = GameSettings::get(settings.eGame());
    const auto &bsaSuffix    = games.sBSASuffix();
    const auto &bsaTexSuffix = games.sBSATexturesSuffix().value_or(bsaSuffix);

    const QString &suffix = type == TexturesBsa ? bsaTexSuffix : bsaSuffix;
    path                  = folder + "/" + PluginsOperations::findPlugin(folder, games) + suffix;
    PLOG_VERBOSE << "Named " << type << path;
}

void BSA::mergeBSAs(std::vector<BSA> &list, MergeBSATypes mergeTypes)
{
    auto standardBegin = list.begin();

    auto incompressibleBegin = std::partition(standardBegin, list.end(), [](const BSA &val) {
        return val.type == StandardBsa;
    });

    auto texturesBegin = std::partition(incompressibleBegin, list.end(), [](const BSA &val) {
        return val.type == UncompressableBsa;
    });

    auto sortBSADescending = [](auto &&one, auto &&two) { return one.filesSize > two.filesSize; };
    //BSAs small enough to be merged
    auto notMaxSize = [](auto &&left, auto &&right) {
        return left.filesSize + right.filesSize < left.maxSize;
    };

    auto sortMerge = [&sortBSADescending, &notMaxSize](auto begin, auto end) {
        std::sort(begin, end, sortBSADescending);
        return merge_if(begin, end, notMaxSize);
    };

    auto standardEnd       = sortMerge(standardBegin, incompressibleBegin);
    auto incompressibleEnd = sortMerge(incompressibleBegin, texturesBegin);
    auto texturesEnd       = sortMerge(texturesBegin, list.end());

    std::vector<BSA> result;
    result.reserve(list.size());

    //This isn't complicated, but it is necessary to pay attention to the order
    std::move(standardBegin, standardEnd, std::back_inserter(result));

    switch (mergeTypes)
    {
        case MergeBSATypes::Both: {
            std::move(incompressibleBegin, incompressibleEnd, std::back_inserter(result));
            std::move(texturesBegin, texturesEnd, std::back_inserter(result));
            result.erase(sortMerge(result.begin(), result.end()), result.end());
            break;
        }
        case MergeBSATypes::Incompressible: {
            std::move(incompressibleBegin, incompressibleEnd, std::back_inserter(result));
            result.erase(sortMerge(result.begin(), result.end()), result.end());
            std::move(texturesBegin, texturesEnd, std::back_inserter(result));
            break;
        }
        case MergeBSATypes::Textures: {
            std::move(texturesBegin, texturesEnd, std::back_inserter(result));
            result.erase(sortMerge(result.begin(), result.end()), result.end());
            std::move(incompressibleBegin, incompressibleEnd, std::back_inserter(result));
            break;
        }
        default: {
            std::move(incompressibleBegin, incompressibleEnd, std::back_inserter(result));
            std::move(texturesBegin, texturesEnd, std::back_inserter(result));
            break;
        }
    }
    list = result;
}

BSA &BSA::operator+=(const BSA &other)
{
    filesSize += other.filesSize;
    files += other.files;

    if (type != other.type)
    {
        const bool incompressible = type == UncompressableBsa || other.type == UncompressableBsa;
        type                      = incompressible ? UncompressableBsa : StandardBsa;
    }

    return *this;
}

BSA BSA::operator+(const BSA &other) const
{
    BSA copy = *this;
    copy += other;
    return copy;
}

bool BSA::operator==(const BSA &other) const
{
    return path == other.path && filesSize == other.filesSize && files == other.files
           && qFuzzyCompare(maxSize, other.maxSize) && type == other.type && format == other.format;
}
} // namespace CAO
