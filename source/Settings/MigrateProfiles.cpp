/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License= v. 2.0. If a copy of the MPL was not distributed with this
 * file= You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings/Profiles.hpp"

namespace CAO {

bool profileVersionIs5(const QDir &profile)
{
    return profile.exists("profile.ini");
}

void migrate5To6(const QDir &oldProfile, Profile &outProfile)
{
    const QString inFile = oldProfile.absoluteFilePath("profile.ini");
    QSettings profileSettings(inFile, QSettings::IniFormat);

    auto &patterns = outProfile.getPatterns();
    patterns.addPattern(PatternSettings{0, toRegexVector({"*"}, true)});
    auto &gPattern = patterns.get().at(
        0); //TODO will have to change once several patterns share the same priority

    auto &generalSets = outProfile.getGeneralSettings();

    profileSettings.beginGroup("BSA");

    generalSets.eBSAFormat  = static_cast<bsa_archive_type_t>(profileSettings.value("bsaFormat").toInt());
    generalSets.iBSAMaxSize = profileSettings.value("maxBsaUncompressedSize").toDouble();

    generalSets.sBSAExtension = profileSettings.value("bsaExtension").toString();
    generalSets.sBSASuffix = profileSettings.value("bsaSuffix").toString();
    generalSets.bBSATabEnabled = profileSettings.value("bsaEnabled").toBool();

    generalSets.eBSATexturesFormat = static_cast<bsa_archive_type_t>(
        profileSettings.value("bsaTexturesFormat").toInt());
    generalSets.bBSATexturesEnabled = profileSettings.value("hasBsaTextures").toBool();
    generalSets.iBSATexturesMaxSize = profileSettings.value("maxBsaTexturesSize").toDouble();

    generalSets.sBSATexturesSuffix = profileSettings.value("bsaTexturesSuffix").toString();

    profileSettings.endGroup();
    profileSettings.beginGroup("Meshes");

    generalSets.bMeshesTabEnabled = profileSettings.value("meshesEnabled").toBool();
    gPattern.eMeshesFileVersion = static_cast<NiFileVersion>(
        profileSettings.value("meshesFileVersion").toInt());
    gPattern.iMeshesStream = profileSettings.value("meshesStream").toUInt();
    gPattern.iMeshesUser = profileSettings.value("meshesUser").toUInt();
    profileSettings.endGroup();
    profileSettings.beginGroup("Animations");
    generalSets.bAnimationsTabEnabled = profileSettings.value("animationsEnabled").toBool();
    gPattern.eAnimationsFormat = static_cast<hkPackFormat>(profileSettings.value("animationFormat").toInt());

    profileSettings.endGroup();
    profileSettings.beginGroup("Textures");

    generalSets.bTexturesTabEnabled = profileSettings.value("texturesEnabled").toBool();
    gPattern.eTexturesFormat = profileSettings.value("texturesFormat").value<DXGI_FORMAT>();
    bool texturesConvertTga = profileSettings.value("texturesConvertTga").toBool();
    auto texturesUnwantedFormats = profileSettings.value("texturesUnwantedFormats").toList();
    bool texturesCompressInterface = profileSettings.value("texturesCompressInterface").toBool();

    profileSettings.endGroup();

    patterns.addPattern(PatternSettings{1, toRegexVector({"*.tga"}, true)});
    auto &pSetsTGA = patterns.get().at(1);
    pSetsTGA.bTexturesForceConvert = texturesConvertTga;

    patterns.addPattern(PatternSettings{2, toRegexVector({"*/interface/*.dds", "*/interface/*.tga"}, true)});
    auto &pSetsInterface = patterns.get().at(2);
    pSetsInterface.bTexturesForceConvert = texturesCompressInterface;

    std::vector<DXGI_FORMAT> unwantedFormats;
    auto getFormat = [](const QVariant &variant) { return variant.value<DXGI_FORMAT>(); };
    texturesUnwantedFormats >>= pipes::transform(getFormat) >>= pipes::push_back(unwantedFormats);
    gPattern.slTextureUnwantedFormats = unwantedFormats;
}

bool isWhitelisted(const QString &fileName)
{
    constexpr std::array<std::string_view, 6> fileWhitelist{"customHeadparts.txt",
                                                            "customLandscape.txt",
                                                            "DummyPlugin.esp",
                                                            "FilesToNotPack.txt",
                                                            "ignoredMods.txt",
                                                            "isBase"};

    auto beg = fileWhitelist.cbegin();
    auto end = fileWhitelist.cend();
    return std::find(beg, end, fileName.toStdString()) != end;
}

void copyFiles(const QDir &oldProfile, const QDir &newProfile)
{
    QDirIterator it(oldProfile);
    while (it.hasNext())
    {
        it.next();
        const QString &filename = it.fileName();
        if (isWhitelisted(filename))
            QFile::copy(it.filePath(), newProfile.absoluteFilePath(filename));
    }
}

void migrateProfiles(const QDir &oldProfileRoot, const QDir &newProfileRoot)
{
    Profiles profiles(newProfileRoot);
    for (const auto &dir : oldProfileRoot.entryList(QDir::NoDotAndDotDot | QDir::Dirs))
    {
        try
        {
            QDir oldDir{oldProfileRoot.absoluteFilePath(dir)};
            QDir newDir{newProfileRoot.absoluteFilePath(dir)};

            if (!profileVersionIs5(oldDir))
                continue;

            profiles.create(dir);
            auto &profile = profiles.get(dir);
            migrate5To6(oldDir, profile);
            profile.saveToJSON();
            copyFiles(oldDir, newDir);
        }
        catch (const std::exception &e)
        {
            continue;
        }
    }
}
} // namespace CAO
