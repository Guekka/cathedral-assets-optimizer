/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License= v. 2.0. If a copy of the MPL was not distributed with this
 * file= You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings/Profiles.hpp"
#include "Utils/Algorithms.hpp"
#include "Utils/Filesystem.hpp"

namespace CAO {

bool profileVersionIs5(const QDir &profile)
{
    return profile.exists("profile.ini");
}

void migrate5To6(const QDir &oldProfile, Profile &outProfile)
{
    const QString inFile = oldProfile.absoluteFilePath("profile.ini");
    QSettings profileSettings(inFile, QSettings::IniFormat);

    auto gPattern     = PatternSettings{0, {"*"}};
    auto &generalSets = outProfile.getGeneralSettings();

    profileSettings.beginGroup("BSA");

    generalSets.eBSAFormat  = static_cast<bsa_archive_type_t>(profileSettings.value("bsaFormat").toInt());
    generalSets.iBSAMaxSize = profileSettings.value("maxBsaUncompressedSize").toDouble();

    generalSets.sBSAExtension = profileSettings.value("bsaExtension").toString();
    generalSets.sBSASuffix = profileSettings.value("bsaSuffix").toString();

    generalSets.eBSATexturesFormat = static_cast<bsa_archive_type_t>(
        profileSettings.value("bsaTexturesFormat").toInt());
    generalSets.bBSATexturesEnabled = profileSettings.value("hasBsaTextures").toBool();
    generalSets.iBSATexturesMaxSize = profileSettings.value("maxBsaTexturesSize").toDouble();

    generalSets.sBSATexturesSuffix = profileSettings.value("bsaTexturesSuffix").toString();

    profileSettings.endGroup();
    profileSettings.beginGroup("Meshes");

    generalSets.eMeshesFileVersion = static_cast<NiFileVersion>(
        profileSettings.value("meshesFileVersion").toInt());
    generalSets.iMeshesStream = profileSettings.value("meshesStream").toUInt();
    generalSets.iMeshesUser   = profileSettings.value("meshesUser").toUInt();
    profileSettings.endGroup();
    profileSettings.beginGroup("Animations");
    gPattern.eAnimationsFormat = static_cast<hkPackFormat>(profileSettings.value("animationFormat").toInt());

    profileSettings.endGroup();
    profileSettings.beginGroup("Textures");

    gPattern.eTexturesFormat = profileSettings.value("texturesFormat").value<DXGI_FORMAT>();
    bool texturesConvertTga = profileSettings.value("texturesConvertTga").toBool();
    auto texturesUnwantedFormats = profileSettings.value("texturesUnwantedFormats").toList();
    bool texturesCompressInterface = profileSettings.value("texturesCompressInterface").toBool();

    profileSettings.endGroup();

    PatternSettings pSetsTGA{1, {"*.tga"}};
    pSetsTGA.bTexturesForceConvert = texturesConvertTga;

    PatternSettings pSetsInterface{2, {"*/interface/*.dds", "*/interface/*.tga"}};
    pSetsInterface.bTexturesForceConvert = texturesCompressInterface;

    PatternSettings pSetsLodgen{3, {"*/textures/terrain/lodgen/*"}};
    pSetsLodgen.bTexturesMipmaps        = false;
    pSetsLodgen.bTexturesCompress       = false;
    pSetsLodgen.bTexturesForceConvert   = false;
    pSetsLodgen.bTexturesLandscapeAlpha = false;

    std::vector<DXGI_FORMAT> unwantedFormats;
    auto getFormat = [](const QVariant &variant) { return variant.value<DXGI_FORMAT>(); };
    texturesUnwantedFormats >>= pipes::transform(getFormat) >>= pipes::push_back(unwantedFormats);
    gPattern.slTextureUnwantedFormats = unwantedFormats;

    PatternMap &patterns = outProfile.getPatterns();
    patterns.addPattern(gPattern);
    patterns.addPattern(pSetsTGA);
    patterns.addPattern(pSetsInterface);
    patterns.addPattern(pSetsLodgen);
}

void convertFiles5To6(const QDir &oldProfile, const QDir &newProfile, Profile &outProfile)
{
    auto readFile = [](const QString &path) {
        QFile file(path);
        auto list = Filesystem::readFile(file, [](QString &line) { return line.prepend('*').append('*'); });
        return std::move(list) | rx::transform([](QString val) { return val.toStdString(); })
               | rx::to_vector();
    };

    QDirIterator it(oldProfile);
    while (it.hasNext())
    {
        it.next();
        const QString &filename = it.fileName();

        if (filename == "customHeadparts.txt")
            outProfile.getFileTypes().slMeshesHeadparts = readFile(it.filePath());

        else if (filename == "customLandscape.txt")
            outProfile.getFileTypes().slTextureLandscapes = readFile(it.filePath());

        else if (filename == "FilesToNotPack.txt")
            outProfile.getFileTypes().slBSABlacklist = readFile(it.filePath());

        else if (filename == "ignoredMods.txt")
            outProfile.getFileTypes().slModsBlacklist = readFile(it.filePath());

        else if (filename == "isBase")
            outProfile.getGeneralSettings().isBaseProfile = true;

        else if (filename == "DummyPlugin.esp")
            QFile::copy(it.filePath(), newProfile.absoluteFilePath(filename));
    }
}

void addDefaultValues5To6(Profile &outProfile)
{
    auto &ft = outProfile.getFileTypes();

    if (ft.slBSAStandardFiles().empty())
    {
        ft.slBSAStandardFiles = {
            "*.bgem",
            "*.bgsm",
            "*.bto",
            "*.btr",
            "*.btt",
            "*.gid",
            "*.hkx",
            "*.lod",
            "*.lst",
            "*.nif",
            "*.pex",
            "*.png",
            "*.psc",
            "*.seq",
            "*.swf",
            "*.tri",
            "*.txt",
        };
    }

    if (ft.slBSATextureFiles().empty())
    {
        ft.slBSATextureFiles = {"*.dds"};
    }

    if (ft.slBSAUncompressibleFiles().empty())
    {
        ft.slBSAUncompressibleFiles = {"*.fuz", "*.lip", "*.mp3", "*.ogg", "*.wav", "*.xwm", "*.*script"};
    }

    auto blacklist = ft.slModsBlacklist();
    if (!contains(blacklist, "*_separator"))
    {
        blacklist.emplace_back("*_separator");
        ft.slModsBlacklist = blacklist;
    }
}

void migrateProfiles(const QDir &oldProfileRoot, const QDir &newProfileRoot)
{
    auto &profiles = Profiles::getInstance();
    profiles.setDir(newProfileRoot);
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
            convertFiles5To6(oldDir, newDir, profile);
            addDefaultValues5To6(profile);
            profile.saveToJSON();
        }
        catch (const std::exception &e)
        {
            continue;
        }
    }
}
} // namespace CAO
