/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "BSA.hpp"
#include "Commands/Plugins/PluginsOperations.hpp"

namespace CAO {
BSA BSA::getBSA(const BSAType &type, const PatternSettings &settings)
{
    BSA bsa;

    switch (type)
    {
        case StandardBsa:
            bsa.type    = BSAType::StandardBsa;
            bsa.maxSize = settings.iBSAMaxSize();
            bsa.format  = settings.eBSAFormat();
            break;
        case TexturesBsa:
            bsa.type    = BSAType::TexturesBsa;
            bsa.maxSize = settings.iBSATexturesMaxSize();
            bsa.format  = settings.eBSATexturesFormat();
            break;
        case UncompressableBsa:
            bsa.type    = BSAType::UncompressableBsa;
            bsa.maxSize = settings.iBSAMaxSize();
            bsa.format  = settings.eBSAFormat();
            break;
    }
    return bsa;
}

void BSA::name(const QString &folder, const GeneralSettings &settings)
{
    const auto &bsaSuffix    = settings.sBSASuffix();
    const auto &bsaTexSuffix = settings.sBSATexturesSuffix();
    const QString &suffix    = type == TexturesBsa ? bsaTexSuffix : bsaSuffix;
    path                     = folder + "/" + PluginsOperations::findPlugin(folder, settings) + suffix;
    PLOG_VERBOSE << "Named " << type << path;
}

void BSA::mergeBSAs(std::vector<BSA> &list)
{
    //Not merging the same BSAs
    auto notEqual = [](auto &&val1, auto &&val2) { return !(val1 == val2); };

    //BSAs small enough to be merged
    auto notMaxSize = [](auto &&leftBSA, auto &&rightBSA) {
        auto total = leftBSA.filesSize + rightBSA.filesSize;
        return total < leftBSA.maxSize;
    };

    //We can only merge BSAs of the same type but everything can be merged into a standard BSA
    auto goodType = [](auto &&leftBSA, auto &&rightBSA) {
        return leftBSA.type != rightBSA.type && leftBSA.type != StandardBsa;
    };

    auto merge = [](BSA leftBSA, BSA rightBSA) {
        leftBSA += rightBSA;
        rightBSA.files.clear(); //Marking for deletion
    };

    using namespace pipes;

    pipes::combinations(list) >>= filter(notEqual) >>= filter(notMaxSize) >>= filter(goodType) >>= for_each(
        merge);
    //TODO check that the pipes refactoring worked correctly

    //Removing empty BSAs
    auto removeIt = std::remove_if(list.begin(), list.end(), [](BSA &bsa) { return bsa.files.isEmpty(); });
    list.erase(removeIt, list.end());
}

BSA &BSA::operator+(const BSA &other)
{
    filesSize += other.filesSize;
    files += other.files;
    if (type != other.type)
        type = StandardBsa;
    return *this;
}

BSA &BSA::operator+=(const BSA &other)
{
    *this = *this + other;
    return *this;
}
bool BSA::operator==(const BSA &other) const
{
    return path == other.path && filesSize == other.filesSize && files == other.files
           && qFuzzyCompare(maxSize, other.maxSize) && type == other.type && format == other.format;
}
} // namespace CAO
